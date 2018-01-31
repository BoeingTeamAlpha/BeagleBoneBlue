/*
 * Licensed under the 'LICENSE'.
 * See LICENSE file in the project root for full license information.
 */
#ifndef IOFACTORY_H
#define IOFACTORY_H

#include "BoeingSurfaceVehicleControl.h"

namespace VehicleControl {

/**
 * @brief	The Control::IOFactory class creates all of the app's IO. Therefore, EVERY IO pin's creation
 *			must be defined in its respective method. This class just acts as a "factory"
 *			for the Control class to create the IO upon initialization.
 *
 * @note	This class is the only object that "knows" how to create the app's IO,
 *			so every IO point must be defined here.
 */
class Control::IOFactory
{
public:

	/**
	 * @brief fillInputList method fills in all of the app's inputs
	 * @param list reference to the complete list of inputs
	 */
	static void fillInputList( InputList& list );

	/**
	 * @brief fillOutputList method fills in all of the app's outputs
	 * @param list reference to the complete list of outputs
	 */
	static void fillOutputList( OutputList& list );

private:
	/**
	 * All methods that are in private HAVE to stay private,
	 * so the compiler does not auto-generate the following methods.
	 */

	/**
	 * @brief	IOFactory private constructor, so an instance of this class cannot be
	 *			created because all methods are declared static.
	 *
	 * @note	This method is not implemented intentionally.
	 */
	IOFactory();

	/**
	 * @brief	IOFactory private copy constructor, so the compiler does not auto-generate
	 *			one.
	 * @param factory
	 *
	 * @note	This method is not implemented intentionally.
	 */
	IOFactory( const IOFactory& factory );

	/**
	 * @brief operator = private equal overload, so the compiler does not auto-generate
	 *			one.
	 * @param factory
	 * @return
	 *
	 * @note	This method is not implemented intentionally.
	 */
	IOFactory& operator=( const IOFactory& factory );
};

} // namespace VehicleControl

#endif // IOFACTORY_H
