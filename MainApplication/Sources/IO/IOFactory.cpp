#include "IOFactory.h"

#include "InputList.h"
#include "OutputList.h"
#include "ServoList.h"

#include "Input.h"
#include "Output.h"
#include "ServoMotorControl.h"

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
	setup.desiredEdge = Input::Edge::Rising;
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
	setup.number = 80;
	list[ IO::OutputList::ServoPowerEnable ] = new IO::Output( setup );
}

void Control::IOFactory::fillServoList( ServoList& list )
{
	list[ IO::ServoList::LeftDriveMotor ]	= new ServoMotorControl( ServoMotorControl::Motor::One );
	list[ IO::ServoList::RightDriveMotor ]	= new ServoMotorControl( ServoMotorControl::Motor::Two );
}

void Control::IOFactory::destroyInputs( InputList& list )
{
	size_t loopVar = IO::InputList::NUM_INPUTS;

	for ( ; loopVar > 0; loopVar-- )
	{
		delete list[ loopVar - 1 ];
	}
}

void Control::IOFactory::destroyOutputs( OutputList& list )
{
	size_t loopVar = IO::OutputList::NUM_OUTPUTS;

	for ( ; loopVar > 0; loopVar-- )
	{
		delete list[ loopVar - 1 ];
	}
}

void Control::IOFactory::destroyServos( ServoList& list )
{
	size_t loopVar = IO::ServoList::NUM_SERVOS;

	for ( ; loopVar > 0; loopVar-- )
	{
		delete list[ loopVar - 1 ];
	}
}

} // namespace VehicleControl
