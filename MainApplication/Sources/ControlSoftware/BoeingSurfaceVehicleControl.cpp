#include "BoeingSurfaceVehicleControl.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "Input.h"
#include "Output.h"
#include "InputList.h"
#include "OutputList.h"
#include "IOFactory.h"
#include "BluetoothDefinitions.h"

namespace VehicleControl {
//std::string( "30:85:A9:E0:1F:9A" )
Control::Control()
	: _isRunning( true )
	, _inputs( IO::InputList::NUM_INPUTS )
	, _outputs( IO::OutputList::NUM_OUTPUTS )
	, _bluetoothConnectedLED( IO::UserLED::Setup( IO::UserLED::LED::UserTwo ) )
	, _runningLED( IO::UserLED::Setup( IO::UserLED::LED::UserThree ) )
	, _client( peerAdress )
	, _server( peerAdress, localAdress )
	, _red( IO::UserLED::Setup( IO::UserLED::LED::Red ) )
{
	// Fill up the IO
	IOFactory::fillInputList( _inputs );
	IOFactory::fillOutputList( _outputs );

	// set up the signal handlers
	signal( SIGINT, signalHandler );
	signal( SIGTERM, signalHandler );

	// set the running LED to its normal blinking
	_runningLED.setState( IO::UserLED::State::Blinking, -1, 900 );
//	printf("finished constructor\r\n");
}

Control& Control::instance()
{
	static Control inst;
	return inst;
}

Control::~Control()
{
	destroyInputs();
	destroyOutputs();
}

void Control::update()
{
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

void Control::destroyInputs()
{
	size_t loopVar = IO::InputList::NUM_INPUTS;

	for ( ; loopVar > 0; loopVar-- )
	{
		delete _inputs[ loopVar - 1 ];
	}
}

void Control::destroyOutputs()
{
	size_t loopVar = IO::OutputList::NUM_OUTPUTS;

	for ( ; loopVar > 0; loopVar-- )
	{
		delete _outputs[ loopVar - 1 ];
	}
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

} // namespace VehicleControl
