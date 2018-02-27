#include "ServoMotorControl.h"

#include <stdio.h>
#include <unistd.h>

#include "ManagerPRUs.h"
#include "ThreadHelper.h"

// Macros
#define PRU_SERVO_LOOP_INSTRUCTIONS	48	// instructions per PRU servo timer loop
#define PRU_FREQUENCY_IN_MHz ( 200 )
#define TenHertz ( 10.0f )
#define Microseconds ( 1000000.0f )

namespace VehicleControl {
namespace IO {

ServoMotorControl::ServoMotorControl( const Motor::Enum motorNumber )
	: _threadRunning( false )
	, _motorNumber( motorNumber )
	, _numberOfLoops( 0 )
	, _pruPointer( ManagerPRUs::instance().sharedMemoryPointer() )
{
	calculateThreadSleepTime( TenHertz );
	ThreadHelper::startDetachedThread( &_thread, handleServoPulseWidth, &_threadRunning, static_cast< void* >( this ) );
}

ServoMotorControl::~ServoMotorControl()
{
	if ( this->_threadRunning )
	{
		pthread_cancel( this->_thread );
	}
}

void ServoMotorControl::setServoPulseWidth( uint32_t pulseWidth, float frequency )
{
	calculateThreadSleepTime( frequency );

	setServoPulseWidth( pulseWidth );
}

void ServoMotorControl::setServoPulseWidth( uint32_t pulseWidth )
{
	_numberOfLoops = ( pulseWidth * PRU_FREQUENCY_IN_MHz ) / PRU_SERVO_LOOP_INSTRUCTIONS;
}

void ServoMotorControl::setServoFrequency( float frequency )
{
	calculateThreadSleepTime( frequency );
}

void* ServoMotorControl::handleServoPulseWidth( void* objectPointer )
{
	// cast the void* to a pointer to this class
	ServoMotorControl* motorControl = static_cast< ServoMotorControl* >( objectPointer );
	uint32_t* const pruPointer = motorControl->_pruPointer;

	while ( motorControl->_threadRunning )
	{
//		if ( pruPointer[ motorControl->_motorNumber ] == 0 )
		{
			pruPointer[ motorControl->_motorNumber ] = motorControl->_numberOfLoops;
		}

		usleep( motorControl->_sleepPeriod );
	}

	return NULL;
}

void ServoMotorControl::calculateThreadSleepTime( float frequency )
{
	// convert the frequency to time and convert it to microseconds
	_sleepPeriod = uint32_t( ( 1 / frequency ) * Microseconds );
}

} // namespace IO
} // namespace VehicleControl
