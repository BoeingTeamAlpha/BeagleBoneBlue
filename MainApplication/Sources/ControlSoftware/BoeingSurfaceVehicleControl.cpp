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

#define PID_PATH ( "/var/run/BoeingSurfaceVehicle.pid" )

namespace VehicleControl {
//std::string( "30:85:A9:E0:1F:9A" )
Control::Control()
	: Bluetooth::Manager::Interface()
	, _isRunning( true )
	, _inputs( IO::InputList::NUM_INPUTS )
	, _outputs( IO::OutputList::NUM_OUTPUTS )
	, _servos( IO::ServoList::NUM_SERVOS )
	, _bluetoothConnectedLED( LibBBB::IO::UserLED::Setup( LibBBB::IO::UserLED::LED::UserTwo ) )
	, _runningLED( LibBBB::IO::UserLED::Setup( LibBBB::IO::UserLED::LED::UserThree ) )
#if defined( RunBluetooth )
//	, _client( peerAdress )
	, _manager( peerAdress
				, localAdress
				, (Bluetooth::Manager::Interface*)this
				, (Bluetooth::Manager::stateChange)&VehicleControl::Control::stateChange )
#endif
	, _red( LibBBB::IO::UserLED::Setup( LibBBB::IO::UserLED::LED::Red ) )
{
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

	// Fill up the IO
	IOFactory::fillInputList( _inputs );
	IOFactory::fillOutputList( _outputs );
	IOFactory::fillServoList( _servos );

	// set up the signal handlers
	signal( SIGINT, signalHandler );
	signal( SIGTERM, signalHandler );

	// set the running LED to its normal blinking
	_runningLED.setState( LibBBB::IO::UserLED::State::Blinking, -1, 900 );

	_receiveMessageSize = sizeof( _receiveMessage ) / sizeof( _receiveMessage[ 0 ] );
	_sendMessageSize = sizeof( _sendMessage ) / sizeof( _sendMessage[ 0 ] );

	memset( (void*)&_receiveMessage, 0, _receiveMessageSize );
	memset( (void*)&_sendMessage, 0, _sendMessageSize );
}

int Control::stateChange(Bluetooth::Manager::State::Enum newState)
{
	printf("control got new state %u\n", newState );

	if ( newState == Bluetooth::Manager::State::Connected )
	{
		_red.setState( LibBBB::IO::UserLED::State::On );
	}
	else
	{
		_red.setState( LibBBB::IO::UserLED::State::Off );
	}

	return 0;
}

Control& Control::instance()
{
	static Control inst;
	return inst;
}

Control::~Control()
{
	IOFactory::destroyInputs( _inputs );
	IOFactory::destroyOutputs( _outputs );
	IOFactory::destroyServos( _servos );

	// remove the file
	printf( "removed %i %s\n", remove( PID_PATH ), strerror( errno ) );
}

void Control::update()
{
	static unsigned int count;
	if ( _manager.isConnected() )
	{
		ssize_t receivedMessage = _manager.receiveData( _receiveMessage, NumberOfBytesPerReceiveMessage );

		for ( int i = 0; i < receivedMessage; ++i )
		{
			printf("rec = %u\n", _receiveMessage[ i ] );
			_receiveMessage[ i ] = 12;//_receiveMessage[ i ] + 1;
		}

		ssize_t sentMessage = _manager.sendData( _sendMessage, NumberOfBytesPerSendMessage );
		printf("%i sent %i bytes rec %i bytes\n", count, sentMessage, receivedMessage );
		++count;
	}
//	static uint32_t width = 0;

//	static bool firstRun = true;
//	static uint32_t counter = 1;


//	if ( !_servos[ IO::ServoList::LeftDriveMotor ]->isRamping() )
//	{
//		printf("counter is %u\n", counter );
//		_servos[ IO::ServoList::LeftDriveMotor ]->setDutyCycle( counter, 5000 );
//		counter += 100;
//	}
//	if ( firstRun )
//	{
//		_servos[ IO::ServoList::LeftDriveMotor ]->setDutyCycle( 1000 );
//		printf("first %u\n", _servos[ IO::ServoList::LeftDriveMotor ]->dutyCycle() );

//		firstRun = false;
//	}

//	if ( counter == 10 )
//	{
//		printf("switching to duty cycle\n");
//		_servos[ IO::ServoList::LeftDriveMotor ]->setDutyCycle( 1000 );
//	}

//	if ( counter == 10 )
//	{
//		_servos[ IO::ServoList::LeftDriveMotor ]->setDesiredDegree( 5 );
//		_servos[ IO::ServoList::LeftDriveMotor ]->setDutyCycle( 500 );
//		printf("50%% %u\n", _servos[ IO::ServoList::LeftDriveMotor ]->dutyCycle() );
//		counter = 0;
//	}
//	++counter;
//	duty += 100;

//	_servos[ IO::ServoList::LeftDriveMotor ]->setDutyCycle( duty );
//	printf("duty is %i\n", _servos[ IO::ServoList::LeftDriveMotor ]->dutyCycle() );

//	if ( duty >= 1000 )
//	{
//		sleep( 4 );
//		duty = 0;
//	}

//	if ( !_servos[ IO::ServoList::LeftDriveMotor ]->isRamping() )
//	{
//		printf("deg %u\n", width );
//		_servos[ IO::ServoList::LeftDriveMotor ]->setDesiredDegree( width, 1500 );
//		width += 20;
//	}
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

	printf("pid is %i\n", pid );

	fprintf( file, "%i", pid );
//	write( fd, (void*)&pid, 4 );

	fclose( file );
}

} // namespace VehicleControl
