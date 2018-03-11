/*
 * Licensed under the 'LICENSE'.
 * See LICENSE file in the project root for full license information.
 */
#ifndef SERVOLIST_H
#define SERVOLIST_H
namespace VehicleControl {
namespace IO {

/**
 * @brief	The ServoList struct holds the names of all servos used in the application.
 *			This allows the array indexes to be accessed by name, not array position.
 *
 * @note	In order to add servos to the app, add an enum here to describe the servo.
 *			Then, add the proper output construction in IOFactory.cpp's fillServoList method.
 */
struct ServoList
{
	enum Enum
	{
		LeftDriveMotor = 0,
		RightDriveMotor,
		NUM_SERVOS
	};
};

} // namespace IO
} // namespace VehicleControl
#endif // SERVOLIST_H
