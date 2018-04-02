#include "CommunicationProtocolParser.h"

#include <string.h>

#include "Input.h"
#include "Output.h"
#include "ServoControl.h"

#include "InputList.h"
#include "OutputList.h"
#include "ServoList.h"


namespace VehicleControl {
Control::CommunicationProtocolParser::CommunicationProtocolParser( Control* control )
	: _control( control )
{

}

Control::CommunicationProtocolParser::~CommunicationProtocolParser()
{

}

void Control::CommunicationProtocolParser::parseIncomingPackets()
{
	// get the received data
	const uint8_t* const incoming = _control->_manager.receiveData();

	// if there is no data, just return
	if ( incoming == NULL )
	{
		return;
	}

	// create local variables
	int16_t signedData;
	uint16_t unsignedData;

	/// @todo This should be automated in a loop or similar
	// parse the left drive motor's value
	signedData = (int16_t)( ( incoming[ 1 ] << 8 ) | ( incoming[ 0 ] ) );

	// if the motor is going in reverse,
	if ( signedData < 0 )
	{
		// set the proper output on the motor driver board
		_control->_outputs[ IO::OutputList::LeftDriveMotorForwardEnable ]->setValue( LibBBB::IO::Output::Value::Low );
	}
	else // motor is going forward
	{
		// so set the proper driver board output
		_control->_outputs[ IO::OutputList::LeftDriveMotorForwardEnable ]->setValue( LibBBB::IO::Output::Value::High );
	}

	// set the PWM output
	_control->_servos[ IO::ServoList::LeftDriveMotor ]->setDutyCycle( abs( signedData ), 250000 );

	// parse the right drive motor's value
	signedData = (int16_t)( ( incoming[ 3 ] << 8 ) | ( incoming[ 2 ] ) );

	// if the motor is going in reverse,
	if ( signedData < 0 )
	{
		// set the proper output on the motor driver board
		_control->_outputs[ IO::OutputList::RightDriveMotorForwardEnable ]->setValue( LibBBB::IO::Output::Value::High );
	}
	else // motor is going forward
	{
		// so set the proper driver board output
		_control->_outputs[ IO::OutputList::RightDriveMotorForwardEnable ]->setValue( LibBBB::IO::Output::Value::Low );
	}

	// set the PWM output
	_control->_servos[ IO::ServoList::RightDriveMotor ]->setDutyCycle( abs( signedData ), 250000 );

	// create and init the local variables
	size_t servo = 2;
	size_t i = 5;

	// loop through the servos
	for ( ; servo < IO::ServoList::NUM_SERVOS; ++servo )
	{
		// create the 16 bit integer from the byte array
		unsignedData = (uint16_t)( ( incoming[ i ] << 8 ) | ( incoming[ i - 1 ] ) );

		// set the servo's desired duty cycle
		_control->_servos[ servo ]->setDutyCycle( unsignedData, 250000 );

		// increment the byte array position
		i += 2;
	}
}

void Control::CommunicationProtocolParser::sendOutgoingPackets()
{
	// get the pointer to the send message
	uint8_t* const outgoing = _control->_sendMessage;

	// create a local variable to pack the bools
	uint8_t bitPack = 0;

	// clear it
	memset( (void*)outgoing, 0, NumberOfBytesPerSendMessage );

	// create dummy batter percent
	outgoing[ 0 ] = 75;

	// get the value of the metal detector
	bool metalDected = _control->_inputs[ IO::InputList::MetalDetected ]->getValue();
//	bool metalDected = true;

	// if metal has been detected
	if ( metalDected )
	{
		// OR the variable with the mask
		bitPack |= BitMask::MetalDetector;
	}

	// set the byte array to the proper value
	outgoing[ 1 ] = bitPack;

	// send the message
	_control->_manager.sendData( outgoing );
}

} // namespace VehicleControl
