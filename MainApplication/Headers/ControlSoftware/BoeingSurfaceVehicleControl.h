/*
 * Licensed under the 'LICENSE'.
 * See LICENSE file in the project root for full license information.
 */
#ifndef BOEINGSURFACEVEHICLECONTROL_H_
#define BOEINGSURFACEVEHICLECONTROL_H_

#include <vector>
#include "BluetoothInterface.h"
#include "BluetoothDefinitions.h"
//#include "BluetoothServer.h"
#include "UserLED.h"

#define RunBluetooth ( 1 )
// Forwared declarations
namespace LibBBB {
namespace IO {
class Input;
class Output;
class MotorControl;
}
}

namespace VehicleControl {

/**
 * @brief	The Control class is the main object of the application. This object wraps all of the
 *			IO and vehicle control logic.
 */
class Control : public LibBBB::Bluetooth::Manager::Interface
{
private:

	/**
	 * Nested class declaration. This way IOFactory has access to this class's
	 * private member variables. Additionally, this ensures that no other object
	 * can create an instance of the IOFactory.
	 */
	class IOFactory;

	/**
	 * @brief	Control private constructor to ensure the only way to
	 *			construct this class is via the instance() method.
	 */
	Control();

public:

	// Typedefs of the input and output lists
	typedef std::vector< LibBBB::IO::Input* > InputList;
	typedef std::vector< LibBBB::IO::Output* > OutputList;
	typedef std::vector< LibBBB::IO::MotorControl* > ServoList;

private:

	// flag to let main know control is still running.
	// if not, main exits the process.
	bool _isRunning;

	// list of all of the inputs
	InputList _inputs;

	// list of all of the outputs
	OutputList _outputs;

	// list of all of the servos
	ServoList _servos;

	// led that turns on when the bluetooth tablet is connected
	LibBBB::IO::UserLED _bluetoothConnectedLED;

	// this led is a status LED of the program. During normal operation,
	// the led just blinks to signify the app is still running properly.
	LibBBB::IO::UserLED _runningLED;

	uint8_t _receiveMessage[ NumberOfBytesPerReceiveMessage ];
	uint8_t _sendMessage[ NumberOfBytesPerSendMessage ];

	size_t _receiveMessageSize;
	size_t _sendMessageSize;

#if defined( RunBluetooth )
	// bluetooth client and server
	// @todo does these two classes need to be wrapped in a Device class?
//	Bluetooth::Client _client;
	LibBBB::Bluetooth::Manager _manager;
#endif

	int stateChange( LibBBB::Bluetooth::Manager::State::Enum newState );
public:

	// TODO: Remove after testing!!
	LibBBB::IO::UserLED _red;

	/**
	 * @brief	instance method is the Singleton method of creating and accessing this
	 *			class. This ensures there can only be ONE instance of this class.
	 * @return reference to this class.
	 *
	 * @note	https://en.wikipedia.org/wiki/Singleton_pattern
	 */
	static Control& instance();

	/**
	 * Destroys the control variables. This method
	 * loops through the input and output lists calling delete
	 * on all IO, which exports the IO pins.
	 */
	~Control();

	/**
	 * @brief update method that controls the vehicle
	 * @todo Should this class spawn a thread and call this method to
	 *		cyclically update vehicle
	 */
	void update();

	/**
	 * @brief isRunning getter
	 * @return true if the control code is still running, false if it is time
	 *			to kill the process.
	 */
	bool isRunning() const;

	/**
	 * @brief inputs getter
	 * @return const reference to all of the inputs;
	 */
	const InputList& inputs() const;

	/**
	 * @brief outputs getter
	 * @return const or non-const reference to all of the outputs.
	 */
	OutputList& outputs();
	const OutputList& outputs() const;

private:

	/**
	 * @brief	signalHandler method handles all of the signals that come from Linux.
	 *			For instance, this method catches when the user presses Ctrl + C or
	 *			when the user presses X on the terminal window. This method sets the
	 *			_isRunning flag to false, triggering main to exit the app.
	 * @param	signal to be received from Linux
	 */
	static void signalHandler( int signal );

	/**
	 * @brief setServoPower turns on and off the servo rail power
	 * @param on true to run servos, falls to turn them off
	 */
	void setServoPower( bool on );

	/**
	 * @brief addPIDFile method adds the pid to the file
	 */
	void addPIDFile();
};

} // namespace VehicleControl

#endif // BOEINGSURFACEVEHICLECONTROL_H_
