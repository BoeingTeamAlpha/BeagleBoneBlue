#include "MotorControl.h"

#include <cmath>
#include <stdio.h>
#include <unistd.h>

#include "ManagerPRUs.h"
#include "ThreadHelper.h"

// Macros
#define PRU_SERVO_LOOP_INSTRUCTIONS	48	// instructions per PRU servo timer loop
#define PRU_FREQUENCY_IN_MHz ( 200 )
#define Microseconds ( 1000000.0f )
#define MaxPulseWidthSubtractor ( 50 )
#define OneHundredPercentDutyCycle ( 1000 )

namespace VehicleControl {
namespace IO {

MotorControl::MotorControl( const Motor::Enum motorNumber, float frequency )
	: _threadRunning( false )
	, _isRamping( false )
	, _motorNumber( motorNumber )
	, _frequency( frequency )
	, _numberOfLoops( 0 )
	, _finalRampedPulseWidth( 0 )
	, _pruPointer( ManagerPRUs::instance().sharedMemoryPointer() )
	, _rampRate( 0 )
{
	_pruPointer[ _motorNumber ] = 0;
	calculateThreadSleepTime( frequency );
	ThreadHelper::startDetachedThread( &_thread, handleWaveform, &_threadRunning, static_cast< void* >( this ) );
}

MotorControl::~MotorControl()
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

void MotorControl::setDesiredDegree( int32_t /* degree */ )
{
	// do nothing implementation
}

void MotorControl::setDesiredDegree( int32_t /* degree */, uint32_t /* rampTime */ )
{
	// do nothing implementation
}

int32_t MotorControl::desiredDegree() const
{
	return -1;
}

void MotorControl::setPulseWidth( uint32_t pulseWidth )
{
	// TODO: is this necessary?
	printf("setting width %u\n", pulseWidth );
	// check if the pulse width is possible, based on the frequency
//	if ( (uint32_t)( ( 1 / _frequency ) * Microseconds ) > pulseWidth )
//	{
//		printf("returned freq is %f wid is %u\n", _frequency, pulseWidth );
//		return;
//	}

	// width possible, so calculate the loops
	calculateLoops( pulseWidth );
}

void MotorControl::setPulseWidth( uint32_t finalPulseWidth, uint32_t rampTime )
{
	// calculate the relevant values to do error checking
	int32_t positionDelta = finalPulseWidth - pulseWidth();
	uint32_t numberOfThreadLoops = ( rampTime * 1000 ) / _sleepTime;

	// if the position is too close, or the ramp time is less than the frequency
	if ( std::abs( positionDelta ) <= 10 || numberOfThreadLoops == 0 )
	{
		// ignore the method
		return;
	}

	// everything was set to valid values
	// set the ramping flag and final pulse width
	_isRamping = true;
	_finalRampedPulseWidth = finalPulseWidth;

//	printf( "loops are %u\n", numberOfThreadLoops );
	// calculate the pulse adder per thread run
	_rampRate = positionDelta / (int32_t)numberOfThreadLoops;

//	printf("final is %u current is %u delta is %i ramp time is %u sleep time is %u rate is %i\n"
//		   , finalPulseWidth
//		   , pulseWidth()
//		   , positionDelta
//		   , rampTime
//		   , _sleepTime
//		   , _rampRate );

	// set the new pulse width
	setPulseWidth( pulseWidth() + (uint32_t)_rampRate );
}

uint32_t MotorControl::pulseWidth() const
{
	return ( _numberOfLoops * PRU_SERVO_LOOP_INSTRUCTIONS ) / PRU_FREQUENCY_IN_MHz;
}

void MotorControl::setFrequency( float frequency )
{
	_frequency = frequency;
	calculateThreadSleepTime( frequency );
}

bool MotorControl::isRamping() const
{
	return _isRamping;
}

float MotorControl::frequency() const
{
	return _frequency;
}

void MotorControl::setDutyCycle( uint32_t dutyCycle )
{
	// check if 100% duty cycle is desired
	if ( dutyCycle >= OneHundredPercentDutyCycle )
	{
		// hack the PRU since it cannot set a constant high value
		calculateLoops( _sleepTime - MaxPulseWidthSubtractor );
	}
	else // 100% duty is not desired
	{
		// calculate the pulse width for the given duty cycle
		uint32_t pulseWidth = ( ( dutyCycle / _frequency ) ) * 1000;
		calculateLoops( pulseWidth );
	}
}

uint32_t MotorControl::dutyCycle() const
{
	// add the subtractor back to the pulse width to check for 100% duty cycle
	uint32_t width = pulseWidth() + MaxPulseWidthSubtractor;

	// the _sleepTime is used because it is 1/f or the time it take for one period
	// if the width is within the bounds of one period, return 100% duty cycle
	// otherwise, return the formula pulse width * frequency
	return width >= _sleepTime - 5 && width <= _sleepTime + 5
			? OneHundredPercentDutyCycle
			: ( pulseWidth() * _frequency ) / 1000;
}

void* MotorControl::handleWaveform( void* objectPointer )
{
	// cast the void* to a pointer to this class
	MotorControl* motorControl = static_cast< MotorControl* >( objectPointer );
	uint32_t* const pruPointer = motorControl->_pruPointer;

	while ( motorControl->_threadRunning )
	{
		// check if the PRU is done. basically, a mutex for the PRU
		if ( pruPointer[ motorControl->_motorNumber ] == 0 )
		{
			// TODO: fix this! there has to be a better way of doing this...
			// check if the servo is ramping
			if ( motorControl->_isRamping )
			{
				// get a local copy of the current pulse width
				uint32_t pulseWidth = motorControl->pulseWidth();

				// check if the servo values are decrementing or incrementing,
				// and check if the ramp is complete
				if ( motorControl->_rampRate < 0 && pulseWidth <= motorControl->_finalRampedPulseWidth)
				{
					motorControl->_isRamping = false;
				}
				else if ( motorControl->_rampRate > 0 && pulseWidth >= motorControl->_finalRampedPulseWidth )
				{
					motorControl->_isRamping = false;
				}
				else // ramp is not complete, so calculate the loops
				{
//					printf("wid %u rate %i\n", MotorControl->pulseWidth(), MotorControl->_rampRate );
					motorControl->calculateLoops( pulseWidth + motorControl->_rampRate );
				}

				// set the servo to the exact position in the ramp to accomodate
				// values that are not divisible by the period
				if ( !motorControl->isRamping() )
				{
					printf("ramp complete\n");
					motorControl->calculateLoops( motorControl->_finalRampedPulseWidth );
				}
			}

			pruPointer[ motorControl->_motorNumber ] = motorControl->_numberOfLoops;
//			printf("thread called update %i\n", motorControl->_pruPointer[ motorControl->_motorNumber ] );
//			printf("thread called update %i loop %i\n", motorControl->_pruPointer[ motorControl->_motorNumber ], motorControl->_numberOfLoops );
		}

		usleep( motorControl->_sleepTime );
	}

	return NULL;
}

void MotorControl::calculateThreadSleepTime( float frequency )
{
	// convert the frequency to time and convert it to microseconds
	_sleepTime = uint32_t( ( 1 / frequency ) * Microseconds );
	//	printf("sleep time is %i for %f freq\n", _sleepTime, frequency );
}

void MotorControl::calculateLoops( uint32_t pulseWidth )
{
	_numberOfLoops = ( pulseWidth * PRU_FREQUENCY_IN_MHz ) / PRU_SERVO_LOOP_INSTRUCTIONS;
	printf("calc loop = %u\n", _numberOfLoops );
}

} // namespace IO
} // namespace VehicleControl
