#ifndef SERVOMOTORCONTROL_H
#define SERVOMOTORCONTROL_H

namespace VehicleControl {
namespace IO {

// TODO: add functionality somewhere to turn on the servo power rail.
class ServoMotorControl
{
public:

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

	const Motor::Enum _motorNumber;

public:

	ServoMotorControl( const Motor::Enum motorNumber );

	~ServoMotorControl();

	void setServoPulseWidth( int microseconds );

	void setServoDutyCycle( int sdf );
};

} // namespace IO
} // namespace VehicleControl
#endif // SERVOMOTORCONTROL_H
