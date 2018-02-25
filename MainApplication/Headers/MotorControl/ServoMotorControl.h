#ifndef SERVOMOTORCONTROL_H
#define SERVOMOTORCONTROL_H

#include <stdint.h>
#include <pthread.h>

namespace VehicleControl {

namespace IO {

// TODO: add functionality somewhere to turn on the servo power rail.
/**
 * @brief	The ServoMotorControl class controls a PRU controlled servo. It handles
 *			all control of the servo via this class's API. This class spawns a thread
 *			to better independently control the frequency of the signal. This allows
 *			each motor to have a unique frequency to better control the different
 *			types of servos.
 * @note	The default frequency is 10 Hz
 * @todo	Add ranges to this class to allow easy control of hobbyist servos that
 *			are more convenient to use a degree range, not a pulse width range.
 */
class ServoMotorControl
{
public:

	/**
	 * @brief	The Motor struct holds all of the allowable motor channels.
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
	const Motor::Enum _motorNumber;
	uint32_t _numberOfLoops;
	uint32_t* const _pruPointer;
	uint32_t _sleepPeriod;
	pthread_t _thread;

public:

	/**
	 * @brief	ServoMotorControl constructor creates the class, and spawns the
	 *			frequency control thread.
	 * @param motorNumber
	 */
	ServoMotorControl( const Motor::Enum motorNumber );

	/**
	 * @brief	ServoMotorControl destructor stops the thread
	 */
	~ServoMotorControl();

	/**
	 * @brief setServoPulseWidth sets the pulse width and frequency of the signal
	 * @param pulseWidth width of the pulse width in microsecond
	 * @param frequency of the signal in Hz.
	 */
	void setServoPulseWidth( uint32_t pulseWidth, float frequency );

	/**
	 * @brief setServoPulseWidth sets the pulse width and frequency of the signal
	 * @param pulseWidth width of the pulse width in microsecond
	 */
	void setServoPulseWidth( uint32_t pulseWidth );

	/**
	 * @brief setServoFrequency sets the frequency of the signal
	 * @param frequency in hz
	 */
	void setServoFrequency( float frequency );

private:

	/**
	 * @brief handleServoPulseWidth handles the frequency of the signal
	 * @param objectPointer pointer to an instance of this class
	 * @return NULL
	 */
	static void* handleServoPulseWidth( void* objectPointer );

	/**
	 * @brief calculateThreadSleepTime calculates the thread sleep time given
	 *			the frequency.
	 * @param frequency in hz
	 */
	void calculateThreadSleepTime( float frequency );
};

} // namespace IO
} // namespace VehicleControl
#endif // SERVOMOTORCONTROL_H
