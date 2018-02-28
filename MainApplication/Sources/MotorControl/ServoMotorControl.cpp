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
#define MaxPRUTime ( 12000000 )
#define ThreadSubtractionTime ( 25 )

namespace VehicleControl {
namespace IO {

ServoMotorControl::ServoMotorControl( const Motor::Enum motorNumber )
	: _threadRunning( false )
	, _motorNumber( motorNumber )
	, _numberOfLoops( 0 )
	, _pruPointer( ManagerPRUs::instance().sharedMemoryPointer() )
{
	_pruPointer[ _motorNumber ] = 0;
	_sleepTime = calculateThreadSleepTime( TenHertz );
	ThreadHelper::startDetachedThread( &_thread, handleServoPulseWidth, &_threadRunning, static_cast< void* >( this ) );
}

ServoMotorControl::~ServoMotorControl()
{
	if ( _pruPointer != NULL )
	{
		_pruPointer[ _motorNumber ] = 0;
	}

	if ( this->_threadRunning )
	{
		pthread_cancel( this->_thread );
	}
}

void ServoMotorControl::setServoPulseWidth( uint32_t pulseWidth, float frequency )
{
	uint32_t sleepTime = calculateThreadSleepTime( frequency );

	checkFor100PercentDutyCycle( pulseWidth, sleepTime );
}

void ServoMotorControl::setServoPulseWidth( uint32_t pulseWidth )
{
	checkFor100PercentDutyCycle( pulseWidth, _sleepTime );
}

void ServoMotorControl::setServoFrequency( float frequency )
{
	_sleepTime = calculateThreadSleepTime( frequency );
}

void* ServoMotorControl::handleServoPulseWidth( void* objectPointer )
{
	// cast the void* to a pointer to this class
	ServoMotorControl* motorControl = static_cast< ServoMotorControl* >( objectPointer );
	uint32_t* const pruPointer = motorControl->_pruPointer;

	while ( motorControl->_threadRunning )
	{
		if ( pruPointer[ motorControl->_motorNumber ] == 0 )
		{
			pruPointer[ motorControl->_motorNumber ] = motorControl->_numberOfLoops;
//			printf("thread called update %i\n", motorControl->_pruPointer[ motorControl->_motorNumber ] );
//			printf("thread called update %i loop %i\n", motorControl->_pruPointer[ motorControl->_motorNumber ], motorControl->_numberOfLoops );
		}

		usleep( motorControl->_sleepTime );
	}

	return NULL;
}

uint32_t ServoMotorControl::calculateThreadSleepTime( float frequency )
{
	// convert the frequency to time and convert it to microseconds
	return uint32_t( ( 1 / frequency ) * Microseconds );
}

void ServoMotorControl::checkFor100PercentDutyCycle( uint32_t pulseWidth, uint32_t sleepTime )
{
	if ( pulseWidth >= sleepTime )
	{
		uint32_t max = MaxPRUTime;
		_numberOfLoops = calculateServoLoops( max );
		_sleepTime = max - ThreadSubtractionTime;
	}
	else
	{
		_numberOfLoops = calculateServoLoops( pulseWidth );
		_sleepTime = sleepTime;
	}
}

uint32_t ServoMotorControl::calculateServoLoops( uint32_t pulseWidth )
{
	return ( pulseWidth * PRU_FREQUENCY_IN_MHz ) / PRU_SERVO_LOOP_INSTRUCTIONS;
}

} // namespace IO
} // namespace VehicleControl
