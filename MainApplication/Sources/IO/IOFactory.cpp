#include "IOFactory.h"

#include "InputList.h"
#include "OutputList.h"
#include "Input.h"
#include "Output.h"

#define SixteenthSecondDebounceTime ( 62 )
#define EighthSecondDebounceTime ( 125 )

namespace VehicleControl {

using namespace IO;

int callback( int /* message */ )
{
	UserLED& led = Control::instance()._red;

//	printf("callback registered on pin %d\r\n", led.state() );
	switch ( led.state() )
	{
	case UserLED::State::Blinking:
		led.setState( UserLED::State::Off );
//		printf("blinking\r\n");
		break;

	case UserLED::State::Off:
		led.setState( UserLED::State::On );
//		printf("off\r\n");
		break;

	case UserLED::State::On:
		led.setState( UserLED::State::Blinking, -1, 60 );
//		printf("on\r\n");
		break;
	}
	return 0;
//	printf("left callback\r\n");
}

IO::Input* createRisingEdgeThreadedInput( int number )
{
	Input::Setup setup;
	setup.callback = &callback;
	setup.debounceTime = EighthSecondDebounceTime;
	setup.desiredEdge = Input::Edge::RISING;
	setup.isActiveLow = true;
	setup.number = number;

	return new Input( setup );
}

void Control::IOFactory::fillInputList( InputList& list )
{
	list[ IO::InputList::MetalDetector ] = createRisingEdgeThreadedInput( 68 );
	list[ IO::InputList::Input1 ] = createRisingEdgeThreadedInput( 57 );
	list[ IO::InputList::Input2 ] = createRisingEdgeThreadedInput( 116 );
	list[ IO::InputList::Input3 ] = createRisingEdgeThreadedInput( 113 );
	list[ IO::InputList::Input4 ] = createRisingEdgeThreadedInput( 98 );
}

void Control::IOFactory::fillOutputList( OutputList& list )
{
	IO::Output::Setup setup;
	setup.number = 49;
	list[ IO::OutputList::LeftDriveMotorForwardEnable ] = new IO::Output( setup );
}

} // namespace VehicleControl
