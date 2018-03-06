#include "MotorControl.h"

#include <cmath>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "ManagerPRUs.h"
#include "ThreadHelper.h"

// Macros
#define PRU_SERVO_LOOP_INSTRUCTIONS	( 48 )	// instructions per PRU servo timer loop
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
	, _currentPulseWidth( 0 )
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
	// TODO: is this necessary? should it be the callers job to set
	// it correctly?
	// check if the pulse width is possible, based on the frequency
//	if ( (uint32_t)( ( 1 / _frequency ) * Microseconds ) > pulseWidth )
//	{
//		printf("returned freq is %f wid is %u\n", _frequency, pulseWidth );
//		return;
//	}

	// clear the ramping flag
	_isRamping = false; // be wary...

	// width possible, so calculate the loops
	calculateLoops( pulseWidth );
}

void MotorControl::setPulseWidth( uint32_t finalPulseWidth, uint32_t rampTime )
{
	// calculate the relevant values to do error checking
	uint32_t currentPulseWidth = pulseWidth();
	int32_t pulseDelta = finalPulseWidth - currentPulseWidth;
	uint32_t numberOfThreadLoops = ( rampTime * 1000 ) / _sleepTime;

	printf("num loops is %u\n", numberOfThreadLoops );

	// if the position is too close, or the ramp time is less than the frequency
	if ( std::abs( pulseDelta ) <= 10 || numberOfThreadLoops == 0 )
	{
		setPulseWidth( finalPulseWidth );
		return;
	}


//	int32_t loopRemainder = ( rampTime * 1000 ) % _sleepTime;
	float integerPart;
	int32_t loopRemainder = int32_t( modf( float( rampTime * 1000.0f ) / float( _sleepTime ), &integerPart ) * 10 );

	printf("loop rem is %i\n", loopRemainder );
	// everything was set to valid values
	// set the ramping flag and final pulse width
	_finalRampedPulseWidth = finalPulseWidth;

	// calculate the pulse width adder per thread run
	_rampRate = pulseDelta / (int32_t)numberOfThreadLoops;
	printf("set ramp rate to %d delta is %d cur is %u final is %u ", _rampRate, pulseDelta, currentPulseWidth, _finalRampedPulseWidth );

	if ( float( finalPulseWidth % _rampRate ) == 0 )
	{
		// set the new pulse width
		printf( "in perfect divisor, setting %d\n", currentPulseWidth + _rampRate );
		setPulseWidth( currentPulseWidth + _rampRate + loopRemainder );
		_isRamping = true; // be wary...
	}
	else
	{
		int32_t remainder = finalPulseWidth - ( ( finalPulseWidth / _rampRate ) * _rampRate );
		printf("in imperfect divisor setting width of %d\n", remainder + currentPulseWidth + 1/* + loopRemainder */);
		setPulseWidth( currentPulseWidth + 1 );
		_isRamping = true; // be wary...
	}
}

uint32_t MotorControl::pulseWidth() const
{
	return _currentPulseWidth;
}

void MotorControl::setFrequency( float frequency )
{
	_frequency = frequency;
	calculateThreadSleepTime( frequency );
}

float MotorControl::frequency() const
{
	return _frequency;
}

bool MotorControl::isRamping() const
{
	return _isRamping;
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
		uint32_t pulseWidth = ( dutyCycle / _frequency ) * 1000;
		calculateLoops( pulseWidth );
	}
}

void MotorControl::setDutyCycle( uint32_t dutyCycle, uint32_t rampTime )
{

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
//		printf("in thread ramp is%i\n", motorControl->isRamping() );
//		printf("!!!in thread %i\n", pruPointer[ motorControl->_motorNumber ] );
		// check if the PRU is done. basically, a mutex for the PRU
		if ( pruPointer[ motorControl->_motorNumber ] == 0 )
		{
//			printf("@@@@in thread %i\n", pruPointer[ motorControl->_motorNumber ] );
			// TODO: fix this! there has to be a better way of doing this...
			// check if the servo is ramping
			if ( motorControl->_isRamping )
			{
				// get a local copy of the current pulse width
				uint32_t currentPulseWidth = motorControl->pulseWidth();
				int32_t nextPulseWidth = currentPulseWidth + motorControl->_rampRate;

				if ( nextPulseWidth == (int32_t)motorControl->_finalRampedPulseWidth )
				{
					motorControl->_isRamping = false;
				}

				printf("next is %i final is %i ramp is %u\n", nextPulseWidth, (int32_t)motorControl->_finalRampedPulseWidth, motorControl->_isRamping );
				motorControl->calculateLoops( nextPulseWidth );

				if ( !motorControl->_isRamping )
				{
					printf("ramp complete\n");
//					motorControl->calculateLoops( motorControl->_finalRampedPulseWidth );
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
	_currentPulseWidth = pulseWidth;
	_numberOfLoops = ( pulseWidth * PRU_FREQUENCY_IN_MHz ) / PRU_SERVO_LOOP_INSTRUCTIONS;
//	printf("calc loop = %u\n", _numberOfLoops );
}

} // namespace IO
} // namespace VehicleControl
