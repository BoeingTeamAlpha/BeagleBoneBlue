#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H

#include "MotorControl.h"
#include "LinearConverter.h"

namespace VehicleControl {
namespace IO {

/**
 * @brief	The ServoControl class controls a servo motor. For convenience, this class adds API to
 *			control the servo in degrees instead of by pulse width or duty cycle. This allows
 *			easier control of the servo.
 *
 * @note	This class can be controlled with pulse width and duty cycle if desired.
 *			The extra methods are just here for convenience.
 */
class ServoControl : public MotorControl
{
public:

	// typedef the appropriate range for the class
	typedef Math::Range< int32_t > RangeType;

private:

	// typedef the converter for easier instantiation
	typedef Math::LinearConverter< int32_t > Converter;

	// member converter
	Converter _pulseWidthConverter;

public:

	/**
	 * @brief ServoControl constructor
	 * @param motorNumber motor to control
	 * @param frequency in Hz
	 * @param degreeRange range of the servo in degrees
	 * @param pulseRange range of the servo in pulse width.
	 * @note	For example, a 180 degree servo that has a minimum pulse width
	 *			of 1500 us and a maximum pulse width of 2500 us would take
	 *			arguments of MotorControl( xxx, xxx, ServoControl::RangeType( 0, 180 ), ServoControl::RangeType( 1500, 2500 ) ).
	 *			Where the xxx's are the motor number and desired frequency.
	 */
	ServoControl( const Motor::Enum motorNumber
				  , float frequency
				  , const RangeType& degreeRange
				  , const RangeType& pulseRange );

	~ServoControl();

	/**
	 * @brief setDesiredDegree method sets the desired servo position in degrees
	 * @param degree position in degrees
	 */
	void setDesiredDegree( int32_t degree );

	/**
	 * @brief setDesiredDegree method sets the desired servo position in degrees
	 * @param degree position in degrees
	 * @param rampTime in milliseconds
	 */
	void setDesiredDegree( int32_t degree, uint32_t rampTime );

	/**
	 * @brief desiredDegree getter
	 * @return desired degree
	 */
	int32_t desiredDegree() const;
};

} // namespace IO
} // namespace VehicleControl
#endif // SERVOCONTROL_H