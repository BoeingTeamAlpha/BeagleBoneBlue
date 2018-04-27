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

Input* createRisingEdgeThreadedInput( int number )
{
	Input::Setup setup;
//	setup.callback = &callback;
	setup.debounceTime = EighthSecondDebounceTime;
	setup.desiredEdge = Input::Edge::Rising;
	setup.isActiveLow = true;

	return new Input( number, setup );
}

void Control::IOFactory::fillInputList( InputList& list )
{
	list[ IO::InputList::MetalDetected ] = createRisingEdgeThreadedInput( 116 );
}

void Control::IOFactory::fillOutputList( OutputList& list )
{
	list[ IO::OutputList::ServoPowerEnable ] = new Output( 80, Output::Setup() );
	list[ IO::OutputList::LeftDriveMotorForwardEnable ] = new Output( 57, Output::Setup() );
	list[ IO::OutputList::RightDriveMotorForwardEnable ] = new Output( 49, Output::Setup() );
}

void Control::IOFactory::fillServoList( ServoList& list )
{
	list[ IO::ServoList::LeftDriveMotor ]
			= new MotorControl( MotorControl::Motor::One, 20.0f, MotorControl::RangeType( 0, 50000 ) );
	list[ IO::ServoList::RightDriveMotor ]
			= new MotorControl( MotorControl::Motor::Two, 20.0f, MotorControl::RangeType( 0, 50000 )  );
	list[ IO::ServoList::CameraLeftRight ]
			= new ServoControl( MotorControl::Motor::Three, 20.0f, ServoControl::RangeType( 0, 180 ), ServoControl::RangeType( 800, 2500 ) );
	list[ IO::ServoList::DumpBedRaiseLower ]
			= new ServoControl( MotorControl::Motor::Four, 20.0f, ServoControl::RangeType( 0, 180 ), ServoControl::RangeType( 800, 2500 ) );
	list[ IO::ServoList::GripperRaiseLower ]
			= new ServoControl( MotorControl::Motor::Five, 20.0f, ServoControl::RangeType( 0, 160 ), ServoControl::RangeType( 1400, 2550 ) );
	list[ IO::ServoList::GripperOpenClose ]
			= new ServoControl( MotorControl::Motor::Six, 20.0f, ServoControl::RangeType( 0, 160 ), ServoControl::RangeType( 1400, 2550 ) );
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
