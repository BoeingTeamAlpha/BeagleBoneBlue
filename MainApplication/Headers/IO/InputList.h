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
		MetalDetector = 0,
		Input1,
		Input2,
		Input3,
		Input4,
		NUM_INPUTS
	};
};

} // namespace IO
} // namespace VehicleControl

#endif // INPUTLIST_H
