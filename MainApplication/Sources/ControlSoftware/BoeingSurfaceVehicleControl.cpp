#include "BoeingSurfaceVehicleControl.h"

#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "Input.h"
#include "MotorControl.h"
#include "ServoControl.h"
#include "Output.h"

#include "InputList.h"
#include "OutputList.h"
#include "ServoList.h"
#include "IOFactory.h"
#include "BluetoothDefinitions.h"
#include "BluetoothManager.h"

#include "CommunicationProtocolParser.h"

#include <time.h>

#include <sstream>
#include <fstream>
#include <iosfwd>

#define PID_PATH ( "/var/run/BoeingSurfaceVehicle.pid" )
#define LOG_PATH ( "Log.txt" )

namespace VehicleControl {

Control::Control()
	: LibBBB::Bluetooth::Manager::Interface()
	, _isRunning( true )
	, _inputs( IO::InputList::NUM_INPUTS )
	, _outputs( IO::OutputList::NUM_OUTPUTS )
	, _servos( IO::ServoList::NUM_SERVOS )
	, _bluetoothConnectedLED( LibBBB::IO::UserLED::Setup( LibBBB::IO::UserLED::LED::Green ) )
	, _red( LibBBB::IO::UserLED::Setup( LibBBB::IO::UserLED::LED::Red ) )
	, _runningLED( LibBBB::IO::UserLED::Setup( LibBBB::IO::UserLED::LED::UserThree ) )
	, _manager( peerAdress
				, localAdress
				, NumberOfBytesPerReceiveMessage
				, NumberOfBytesPerSendMessage
				, BluetoothPollRate
				, (LibBBB::Bluetooth::Manager::Interface*)this
				, (LibBBB::Bluetooth::Manager::stateChange)&VehicleControl::Control::stateChange )
{
	// Fill up the IO
	IOFactory::fillInputList( _inputs );
	IOFactory::fillOutputList( _outputs );
	IOFactory::fillServoList( _servos );

	// check if the file exists
	if ( access( PID_PATH, F_OK ) == 0 )
	{
		// it does, so we did not shut down properly
		// get the pid of the running process
		std::ifstream inputStream;
		std::string input;
		inputStream.open( PID_PATH );
		std::getline( inputStream, input );
		inputStream.close();

		printf("killing pid %i\n", std::atoi( input.c_str() ));
		// kill old process
		kill( (pid_t)std::atoi( input.c_str() ), SIGTERM );

		// remove the file
		remove( PID_PATH );

		// app does not like running after forcing another
		// instance to close, so just shut this instance down
		// as well.
		_isRunning = false;
		return;
	}

	// add pid file
	addPIDFile();

	// set up the signal handlers
	signal( SIGINT, signalHandler );
	signal( SIGTERM, signalHandler );

	_parser = new CommunicationProtocolParser( this );

	// set the running LED to its normal blinking
	_runningLED.setState( LibBBB::IO::UserLED::State::Blinking, -1, 900 );

	srand( time( NULL) );

	time_t timer;
	char buffer[ 26 ];
	struct tm* tmInfo;

	time( &timer );
	tmInfo = localtime( &timer );

	strftime( buffer, 26, "%Y-%m-%d %H:%M:%S", tmInfo );

	FILE* file = fopen( LOG_PATH, "a" );
	if ( file == NULL )
	{
		fprintf( stderr, "Cannot access log file\n" );
		exit( EXIT_FAILURE );
	}

	fprintf( file, "\nNew log time is: %s\n", buffer );

	fclose( file );

	turnOffAllMotors();

	_red.setState( LibBBB::IO::UserLED::State::On );
}

Control::~Control()
{
	IOFactory::destroyInputs( _inputs );
	IOFactory::destroyOutputs( _outputs );
	IOFactory::destroyServos( _servos );

	delete _parser;

	// remove the file
	remove( PID_PATH );
}

int Control::stateChange( LibBBB::Bluetooth::Manager::State::Enum newState)
{
	time_t timer;
	char buffer[ 26 ];
	std::ostringstream message;
	std::fstream file;
	struct tm* tmInfo;

	time( &timer );
	tmInfo = localtime( &timer );

	strftime( buffer, 26, "%Y-%m-%d %H:%M:%S", tmInfo );
	message << buffer;

	if ( newState == LibBBB::Bluetooth::Manager::State::Connected )
	{
		setServoPower( true );
		_red.setState( LibBBB::IO::UserLED::State::Off );
		_bluetoothConnectedLED.setState( LibBBB::IO::UserLED::State::Blinking, -1, 900 );
		_servos[ IO::ServoList::LeftDriveMotor ]->setDutyCycle( 0, BluetoothPollRate  );
		_servos[ IO::ServoList::RightDriveMotor ]->setDutyCycle( 0, BluetoothPollRate );

		message << ": connected\n";
	}
	else
	{
		_servos[ IO::ServoList::LeftDriveMotor ]->setDutyCycle( 0, BluetoothPollRate  );
		_servos[ IO::ServoList::RightDriveMotor ]->setDutyCycle( 0, BluetoothPollRate );
		_red.setState( LibBBB::IO::UserLED::State::On );
		_bluetoothConnectedLED.setState( LibBBB::IO::UserLED::State::Off );
		message << ": disconnected\n";
		setServoPower( false );
	}

	file.open( LOG_PATH, std::fstream::out | std::fstream::app );

	file << message.str();
	file.close();
	return 0;
}

Control& Control::instance()
{
	static Control inst;
	return inst;
}

void Control::update()
{
	if ( _manager.isConnected() )
	{
		_parser->parseIncomingPackets();
		_parser->sendOutgoingPackets();
	}
	else
	{
		_servos[ IO::ServoList::LeftDriveMotor ]->setDutyCycle( 0 );
		_servos[ IO::ServoList::RightDriveMotor ]->setDutyCycle( 0 );
	}
}

bool Control::isRunning() const
{
	return _isRunning;
}

const Control::InputList& Control::inputs() const
{
	return _inputs;
}

Control::OutputList& Control::outputs()
{
	return _outputs;
}

const Control::OutputList& Control::outputs() const
{
	return _outputs;
}

void Control::signalHandler( int signal )
{
	Control& control = Control::instance();

	switch ( signal )
	{
	case SIGINT:
		printf( "Caught Ctrl + C\r\n" );
		control._isRunning = false;
		break;

	case SIGTERM:
		printf( "Caught Terminate\r\n" );
		control._isRunning = false;
		break;

	default:
		control._isRunning = false;
		break;
	}
}

void Control::setServoPower( bool on )
{
	_outputs[ IO::OutputList::ServoPowerEnable ]->setValue( ( LibBBB::IO::Output::Value::Enum ) on );
}

void Control::addPIDFile()
{
	FILE* file = fopen( PID_PATH, "w+" );

	if ( file == NULL )
	{
		fprintf( stderr, "Cannot create PID file\n" );
		exit( EXIT_FAILURE );
	}

	pid_t pid = getpid();

	fprintf( file, "%i", pid );

	fclose( file );
}

void Control::turnOffAllMotors()
{
	// loop through the servos
	for ( size_t motor = 0; motor < IO::ServoList::NUM_SERVOS; ++motor )
	{
		// set the servo's desired duty cycle
		_servos[ motor ]->setDutyCycle( 0 );
	}
}

} // namespace VehicleControl
