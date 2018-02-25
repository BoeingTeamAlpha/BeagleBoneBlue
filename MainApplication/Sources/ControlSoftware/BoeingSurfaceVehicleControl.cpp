#include "BoeingSurfaceVehicleControl.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "Input.h"
#include "Output.h"
#include "ServoMotorControl.h"

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

	_servos[ IO::ServoList::LeftDriveMotor ]->setServoPulseWidth( 500000 );
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
