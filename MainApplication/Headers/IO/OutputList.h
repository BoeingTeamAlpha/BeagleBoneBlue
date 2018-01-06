#ifndef OUTPUTLIST_H
#define OUTPUTLIST_H

namespace VehicleControl {
namespace IO {

/**
 * @brief	The OutputList struct wraps the enum of all of the outputs used in the application.
 * @note	In order to add outputs to the app, add an enum here to describe the output.
 *			Then, add the proper output construction in IOFactory.cpp's fillOutputList method.
 */
struct OutputList
{
	enum Enum
	{
		LeftDriveMotorForwardEnable = 0,
//		LeftDriveMotorReverseEnable = 1,
//		LeftDriveMotorPWMSignal,
//		RightDriveMotorForwardEnable,
//		RightDriveMotorReverseEnable,
//		RightDriveMotorPWMSignal,
		NUM_OUTPUTS
	};
};

} // namespace IO
} // namespace VehicleControl

#endif // OUTPUTLIST_H
