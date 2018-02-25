#include "ServoMotorControl.h"

#include "ManagerPRUs.h"

#define PRU_SERVO_LOOP_INSTRUCTIONS	48	// instructions per PRU servo timer loop

namespace VehicleControl {
namespace IO {

ServoMotorControl::ServoMotorControl( const Motor::Enum motorNumber )
    : _motorNumber( motorNumber )
{

}

ServoMotorControl::~ServoMotorControl()
{

}

void ServoMotorControl::setServoPulseWidth( int microseconds )
{
    uint32_t* const pruPointer = ManagerPRUs::instance().sharedMemoryPointer();
	uint32_t numberOfLoops = ( microseconds * 200.0 ) / PRU_SERVO_LOOP_INSTRUCTIONS;

    pruPointer[ _motorNumber ] = numberOfLoops;
}

} // namespace IO
} // namespace VehicleControl
