#include "BoeingSurfaceVehicleControl.h"

#include <signal.h>
#include <stdlib.h>
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


// TODO: add logic to store the PID of our app, so we
// can tell if there is another running instance of ourselves
namespace VehicleControl {
//std::string( "30:85:A9:E0:1F:9A" )
Control::Control()
	: _isRunning( true )
	, _inputs( IO::InputList::NUM_INPUTS )
	, _outputs( IO::OutputList::NUM_OUTPUTS )
	, _servos( IO::ServoList::NUM_SERVOS )
	, _bluetoothConnectedLED( IO::UserLED::Setup( IO::UserLED::LED::UserTwo ) )
	, _runningLED( IO::UserLED::Setup( IO::UserLED::LED::UserThree ) )
#if defined( RunBluetooth )
	, _client( peerAdress )
	, _server( peerAdress, localAdress )
#endif
	, _red( IO::UserLED::Setup( IO::UserLED::LED::Red ) )
{
	// Fill up the IO
	IOFactory::fillInputList( _inputs );
	IOFactory::fillOutputList( _outputs );
	IOFactory::fillServoList( _servos );

	// set up the signal handlers
	signal( SIGINT, signalHandler );
	signal( SIGTERM, signalHandler );

	// set the running LED to its normal blinking
	_runningLED.setState( IO::UserLED::State::Blinking, -1, 900 );

//	_servos[ IO::ServoList::LeftDriveMotor ]->setPulseWidth( 99999 );
//	printf("pulse is %u\n", _servos[ IO::ServoList::LeftDriveMotor ]->pulseWidth() );
//	_servos[ IO::ServoList::LeftDriveMotor ]->setPulseWidth( 2500, 1000 );
//	printf("duty is %i\n"
//		   , _servos[ IO::ServoList::LeftDriveMotor ]->dutyCycle() );
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
}

void Control::update()
{
#if defined( RunBluetooth )
	static bool isBluetoothConnected = false;
	if ( isBluetoothConnected != this->_server.isConnected() )
	{
		printf("BT changed states\r\n");
		isBluetoothConnected = this->_server.isConnected();

		IO::UserLED::State::Enum state = isBluetoothConnected
				? IO::UserLED::State::On
				: IO::UserLED::State::Off;

		_bluetoothConnectedLED.setState( state );
	}
#endif

//	static uint32_t width = 0;

	static bool firstRun = true;
	static uint32_t counter = 1;


	if ( !_servos[ IO::ServoList::LeftDriveMotor ]->isRamping() )
	{
		printf("counter is %u\n", counter );
		_servos[ IO::ServoList::LeftDriveMotor ]->setDutyCycle( counter, 5000 );
		counter += 100;
	}
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
	_outputs[ IO::OutputList::ServoPowerEnable ]->setValue( ( IO::Output::Value::Enum ) on );
}

} // namespace VehicleControl
