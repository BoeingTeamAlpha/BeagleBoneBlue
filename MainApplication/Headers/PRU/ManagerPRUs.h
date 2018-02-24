#ifndef MANAGERPRUS_H
#define MANAGERPRUS_H

#include <stdint.h>

namespace VehicleControl {

class ManagerPRUs
{
private:

	/**
	 * @brief ManagerPRUs contructs the singleton instance of the class
	 */
	ManagerPRUs();

private:

	bool _pruRunning;

	uint32_t* _pruSharedMemoryPointer;

	int bindOrUnbindPRU( bool bind );

	int mapVirtualAddressSpace();

public:

	/**
	 * @brief ~ManagerPRUs unbinds the PRUs, shutting them down
	 */
	~ManagerPRUs();

	/**
	 * @brief	instance method is the Singleton method of creating and accessing this
	 *			class. This ensures there can only be ONE instance of this class. If there
	 *			are two instances of this class created, it could mess with the virtual
	 *			memory mapping!!
	 * @note	https://en.wikipedia.org/wiki/Singleton_pattern
	 * @return reference to this class.
	 */
	static ManagerPRUs& instance();

	/**
	 * @brief restartPRUs restart the PRUs
	 * @return 0 if successful
	 */
	int restartPRUs();

	/**
	 * @brief setPRUState turns the BOTH PRUs on or off
	 * @param on
	 * @return false for no errors
	 */
	int setPRUState( bool on );

	uint32_t* const sharedMemoryPointer();

	const uint32_t* const sharedMemoryPointer() const;
};

} // namespace VehicleControl
#endif // MANAGERPRUS_h
