/*
 * Licensed under the 'LICENSE'.
 * See LICENSE file in the project root for full license information.
 */
#ifndef INPUTLIST_H
#define INPUTLIST_H

namespace VehicleControl {
namespace IO {

/**
 * @brief	The InputList struct wraps the enum of all of the inputs used in the application.
 * @note	In order to add inputs to the app, add an enum here to describe the input.
 *			Then, add the proper input construction in IOFactory.cpp's fillInputList method.
 */
struct InputList
{
	enum Enum
	{
		Input1 = 0,
		NUM_INPUTS
	};

};

} // namespace IO
} // namespace VehicleControl

#endif // INPUTLIST_H
