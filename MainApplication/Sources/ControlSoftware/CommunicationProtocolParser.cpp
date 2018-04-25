#include "CommunicationProtocolParser.h"

#include <string.h>

#include "Input.h"
#include "Output.h"
#include "ServoControl.h"

#include "InputList.h"
#include "OutputList.h"
#include "ServoList.h"

#include "AnalogToDigitalController.h"

using namespace LibBBB::Math;

namespace VehicleControl {

static const float BatteryLowValue = 0;
static const float BatteryHighValue = 100;

Control::CommunicationProtocolParser::CommunicationProtocolParser( Control* control )
	: _control( control )
	, _batteryConverter( BatteryConverter::RangeType( 0.3, 8.55 ), BatteryConverter::RangeType( BatteryLowValue, BatteryHighValue ) )
{
	memset( &_sendMessage, 0, NumberOfBytesPerSendMessage );
	memset( &_receiveMessage, 0, NumberOfBytesPerReceiveMessage );
}

Control::CommunicationProtocolParser::~CommunicationProtocolParser()
{

}

float Control::CommunicationProtocolParser::boundValue(const float value)
{
	if ( value < BatteryLowValue )
	{
		return BatteryLowValue;
	}

	if ( value > BatteryHighValue )
	{
		return BatteryHighValue;
	}

	return value;
}

void Control::CommunicationProtocolParser::parseIncomingPackets()
{
	const uint8_t* const receiveBuffer = _control->_manager.receiveData();

	// if there is no data, just return
	if ( receiveBuffer == NULL )
	{
		return;
	}

	memcpy( _receiveMessage, receiveBuffer, NumberOfBytesPerReceiveMessage );

	/// create local variables
	int16_t signedData;
	uint16_t unsignedData;

	/// @todo This should be automated in a loop or similar
	// parse the left drive motor's value
	signedData = (int16_t)( ( _receiveMessage[ 1 ] << 8 ) | ( _receiveMessage[ 0 ] ) );


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
	_control->_servos[ IO::ServoList::LeftDriveMotor ]->setDutyCycle( abs( signedData * 10 ), BluetoothPollRate / 1000 );

	// parse the right drive motor's value
	signedData = (int16_t)( ( _receiveMessage[ 3 ] << 8 ) | ( _receiveMessage[ 2 ] ) );

	// if the motor is going in reverse,
	if ( signedData < 0 )
	{
		// set the proper output on the motor driver board
		_control->_outputs[ IO::OutputList::RightDriveMotorForwardEnable ]->setValue( LibBBB::IO::Output::Value::Low );
	}
	else // motor is going forward
	{
		// so set the proper driver board output
		_control->_outputs[ IO::OutputList::RightDriveMotorForwardEnable ]->setValue( LibBBB::IO::Output::Value::High );
	}

	// set the PWM output
	_control->_servos[ IO::ServoList::RightDriveMotor ]->setDutyCycle( abs( signedData * 10 ), BluetoothPollRate / 1000 );

	// create and init the local variables
	size_t servo = 2;
	size_t i = 5;

	// loop through the servos
	for ( ; servo < IO::ServoList::NUM_SERVOS; ++servo )
	{
		// create the 16 bit integer from the byte array
		unsignedData = (uint16_t)( ( _receiveMessage[ i ] << 8 ) | ( _receiveMessage[ i - 1 ] ) );

		// set the servo's desired duty cycle
		_control->_servos[ servo ]->setDesiredDegree( unsignedData, BluetoothPollRate / 1000 );

		// increment the byte array position
		i += 2;
	}

//	// loop through the servos
//	for ( servo = 0; servo < IO::ServoList::NUM_SERVOS; ++servo )
//	{
//		printf("%u ", _control->_servos[ servo ]->dutyCycle() );
//	}

//	printf("\n");
}

void Control::CommunicationProtocolParser::sendOutgoingPackets()
{
//	memset( _sendMessage, 0, NumberOfBytesPerSendMessage );
	// create a local variable to pack the bools
	uint8_t bitPack = 0;

	// clear it
//	memset( (void*)outgoing, 0, NumberOfBytesPerSendMessage );

	// send the battery percent
	_sendMessage[ 0 ] = boundValue( _batteryConverter.convertXtoY( LibBBB::AnalogToDigitalController::instance().batteryVoltage() ) );

	// get the value of the metal detector
	bool metalDected = _control->_inputs[ IO::InputList::MetalDetected ]->getValue();

	// if metal has been detected
	if ( metalDected )
	{
		// OR the variable with the mask
		bitPack |= BitMask::MetalDetector;
	}

	// set the byte array to the proper value
	_sendMessage[ 1 ] = bitPack;

	// send the message
	_control->_manager.sendData( _sendMessage );
}

} // namespace VehicleControl
