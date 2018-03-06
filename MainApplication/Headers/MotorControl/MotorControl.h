#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <stdint.h>
#include <pthread.h>

namespace VehicleControl {
namespace IO {

/**
 * @brief	The MotorControl class controls a PRU controlled servo. It handles
 *			all control of the servo via this class's API. This class spawns a thread
 *			to better independently control the frequency of the signal. This allows
 *			each motor to have a unique frequency to better control the different
 *			types of servos.
 * @todo	Add ranges to this class to allow easy control of hobbyist servos that
 *			are more convenient to use a degree range, not a pulse width range.
 */
class MotorControl
{
public:

	/**
	 * @brief	The Servo struct holds all of the allowable motor channels.
	 * @note	This class also accounts for the motor's being zero based
	 *			programmatically, but 1 based in the "real" world.
	 */
	struct Motor
	{
		enum Enum
		{
			// Zero based offset
			One = 0, Two, Three, Four,
			Five, Six, Seven, Eight
		};
	};

private:

	// Member variables
	bool _threadRunning;
	bool _isRamping;
	const Motor::Enum _motorNumber;
	float _frequency;
	uint32_t _numberOfLoops;
	uint32_t _sleepTime;
	uint32_t _currentPulseWidth;
	uint32_t _finalRampedPulseWidth;
	uint32_t* const _pruPointer;
	int32_t _rampRate;
	pthread_t _thread;

public:

	/**
	 * @brief	ServoControl constructor creates the class, and spawns the
	 *			frequency control thread.
	 * @param	motorNumber
	 */
	MotorControl( const Motor::Enum motorNumber, float frequency );

	/**
	 * @brief	ServoMotorControl destructor stops the thread
	 */
	virtual ~MotorControl();

	/**
	 * @brief setDesiredDegree method sets the desired servo position in degrees
	 * @param degree position in degrees
	 * @note	This method does nothing in this class. It is just here for polymorphic reason.
	 *			If these methods are desired, use the ServoControl class
	 */
	virtual void setDesiredDegree( int32_t degree );

	/**
	 * @brief setDesiredDegree method sets the desired servo position in degrees
	 * @param degree position in degrees
	 * @param rampTime in milliseconds
	 * @note	This method does nothing in this class. It is just here for polymorphic reason.
	 *			If these methods are desired, use the ServoControl class
	 */
	virtual void setDesiredDegree( int32_t degree, uint32_t rampTime );

	/**
	 * @brief desiredDegree getter
	 * @return ALWAYS -1
	 * @note	This method does nothing in this class. It is just here for polymorphic reason.
	 *			If these methods are desired, use the ServoControl class
	 */
	virtual int32_t desiredDegree() const;

	/**
	 * @brief pulseWidth getter
	 * @return pulse width in microseconds
	 */
	uint32_t pulseWidth() const;

	/**
	 * @brief setPulseWidth sets the pulse width of the signal
	 * @param pulseWidth width of the pulse in microsecond
	 * @note if this method is called, it will cancel ramping functionality
	 */
	void setPulseWidth( uint32_t pulseWidth );

	/**
	 * @brief setPulseWidth sets the final pulse width of the signal using the given ramp time
	 * @param pulseWidth width of the pulse width in microsecond
	 * @param rampTime time, in milliseconds, to achieve the final pulse width
	 */
	void setPulseWidth( uint32_t finalPulseWidth, uint32_t rampTime );

	/**
	 * @brief setFrequency sets the frequency of the signal
	 * @param frequency in hz
	 */
	void setFrequency( float frequency );

	/**
	 * @brief frequency getter
	 * @return freqency in hertz
	 */
	float frequency() const;

	/**
	 * @brief isRamping getter
	 * @return true if the servo is ramping, false if it is not
	 */
	bool isRamping() const;

	/**
	 * @brief setDutyCycle setter
	 * @param dutyCycle in percent * 10, so 100% is 1000
	 */
	void setDutyCycle( uint32_t dutyCycle );

	/**
	 * @brief setDutyCycle
	 * @param dutyCycle
	 * @param rampTime
	 */
	void setDutyCycle( uint32_t dutyCycle, uint32_t rampTime );

	/**
	 * @brief dutyCycle getter
	 * @return current duty cycle
	 */
	uint32_t dutyCycle() const;

private:

	/**
	 * @brief handleWaveform handles the waveform of the signal
	 * @param objectPointer pointer to an instance of this class
	 * @return NULL
	 */
	static void* handleWaveform( void* objectPointer );

	/**
	 * @brief calculateThreadSleepTime calculates the thread sleep time given
	 *			the frequency.
	 * @param frequency in hz
	 */
	void calculateThreadSleepTime( float frequency );

	/**
	 * @brief calculateLoops calculates the number of loops given the pulse width
	 * @param pulseWidth in microsecons
	 * @return number of PRU loops
	 */
	void calculateLoops( uint32_t pulseWidth );
};

} // namespace IO
} // namespace VehicleControl
#endif // MOTORCONTROL_H
