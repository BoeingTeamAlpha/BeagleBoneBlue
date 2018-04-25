#ifndef COMMUNICATIONPROTOCOLPARSER_H
#define COMMUNICATIONPROTOCOLPARSER_H

#include "BoeingSurfaceVehicleControl.h"
#include "LinearConverter.h"
#include "BluetoothDefinitions.h"

namespace VehicleControl {

/**
 * @brief	The Control::CommunicationProtocolParser class handles parcing and
 *			packing communication packets. This class is the only one that
 *			"knows" how the protocol is structured and packed. Therefore,
 *			this class, for instance, sets each servo to its desired value.
 *			Additionally, this class packs up all the info that is sent to
 *			the tablet.
 */
class Control::CommunicationProtocolParser
{
private:

	/**
	 * @brief	The BitMask struct contains all of the bit masks for
	 *			packing bools into a single byte to save bandwidth.
	 */
	struct BitMask
	{
		enum Enum
		{
			MetalDetector	= 0x01
		};
	};

private:

	/// pointer to the controls class for easy access to its private
	///	variables
	Control* _control;

	typedef LibBBB::Math::LinearConverter< float > BatteryConverter;
	BatteryConverter _batteryConverter;

	uint8_t _sendMessage[ NumberOfBytesPerSendMessage ];

	uint8_t _receiveMessage[ NumberOfBytesPerReceiveMessage ];

	static inline float boundValue( const float value );

public:

	/**
	 * @brief	CommunicationProtocolParser constructor creates an instance
	 *			of this class.
	 * @param control pointer to the control class
	 */
	CommunicationProtocolParser( Control* control );

	/**
	 * @brief ~CommunicationProtocolParser do nothing destructor
	 */
	~CommunicationProtocolParser();

	/**
	 * @brief	parseIncomingPackets method gets and parses the incoming
	 *			Bluetooth packets. This method calls the Bluetooth manager's
	 *			receive data to get the data, parses the data, and sets
	 *			the associated values to its desired values. Essentially,
	 *			this method does everything to set the incoming data
	 *			to its proper values.
	 */
	void parseIncomingPackets();

	/**
	 * @brief	sendOutgoingPackets packs and send outgoing data. This method
	 *			takes care of packing bools into a single byte, and calling
	 *			the proper method in the Bluetooth manager.
	 */
	void sendOutgoingPackets();
};

} // namespace VehicleControl
#endif // COMMUNICATIONPROTOCOLPARSER_H
