/*
 * Licensed under the 'LICENSE'.
 * See LICENSE file in the project root for full license information.
 */
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
		ServoPowerEnable = 0,
//		LeftDriveMotorForwardEnable,
//		RightDriveMotorForwardEnable,
		NUM_OUTPUTS
	};
};

} // namespace IO
} // namespace VehicleControl

#endif // OUTPUTLIST_H
