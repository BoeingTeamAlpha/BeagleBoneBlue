#include "IOFactory.h"

#include "InputList.h"
#include "OutputList.h"
#include "ServoList.h"

#include "Input.h"
#include "Output.h"
#include "MotorControl.h"
#include "ServoControl.h"

#define SixteenthSecondDebounceTime ( 62 )
#define EighthSecondDebounceTime ( 125 )

using namespace LibBBB::IO;
namespace VehicleControl {

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

Input* createRisingEdgeThreadedInput( int number )
{
	Input::Setup setup;
	setup.callback = &callback;
	setup.debounceTime = EighthSecondDebounceTime;
	setup.desiredEdge = Input::Edge::Rising;
	setup.isActiveLow = true;

	return new Input( number, setup );
}

void Control::IOFactory::fillInputList( InputList& list )
{
	list[ IO::InputList::Input1 ] = createRisingEdgeThreadedInput( 57 );
}

void Control::IOFactory::fillOutputList( OutputList& list )
{
	list[ IO::OutputList::ServoPowerEnable ] = new Output( 80, Output::Setup() );
}

void Control::IOFactory::fillServoList( ServoList& list )
{
	list[ IO::ServoList::LeftDriveMotor ]	= new ServoControl( MotorControl::Motor::One, 5.0f, ServoControl::RangeType( 0, 180 ), ServoControl::RangeType( 800, 2500 ) );
	list[ IO::ServoList::RightDriveMotor ]	= new MotorControl( MotorControl::Motor::Two, 5.0f, MotorControl::RangeType( 2500, 150000 ) );
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
