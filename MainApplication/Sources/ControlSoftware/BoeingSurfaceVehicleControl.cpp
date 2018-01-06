#include "BoeingSurfaceVehicleControl.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "Input.h"
#include "Output.h"
#include "InputList.h"
#include "OutputList.h"
#include "IOFactory.h"

namespace VehicleControl {

Control::Control()
	: _isRunning( true )
	, _inputs( IO::InputList::NUM_INPUTS )
	, _outputs( IO::OutputList::NUM_OUTPUTS )
	, _red( IO::UserLED::Setup( IO::UserLED::LED::Red ) )
	, _runningLED( IO::UserLED::Setup( IO::UserLED::LED::UserThree ) )
{
	// Fill up the IO
	IOFactory::fillInputList( _inputs );
	IOFactory::fillOutputList( _outputs );

	// set up the signal handlers
	signal( SIGINT, signalHandler );
	signal( SIGTERM, signalHandler );

	// set the running LED to its normal blinking
	_runningLED.setState( IO::UserLED::State::Blinking, -1, 480 );
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

}
