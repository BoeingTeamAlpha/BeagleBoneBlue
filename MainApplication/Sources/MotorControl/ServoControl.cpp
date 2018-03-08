#include "ServoControl.h"

namespace VehicleControl {
namespace IO {

ServoControl::ServoControl( const MotorControl::Motor::Enum motorNumber
							, float frequency
							, const ServoControl::RangeType& degreeRange
							, const ServoControl::RangeType& pulseRange )
	: MotorControl( motorNumber, frequency )
	, _pulseWidthConverter( degreeRange, pulseRange )
{

}

ServoControl::~ServoControl()
{

}

void ServoControl::setDesiredDegree( int32_t degree )
{
	MotorControl::setPulseWidth( (uint32_t)_pulseWidthConverter.convertXtoY( degree ) );
}

void ServoControl::setDesiredDegree( int32_t degree, uint32_t rampTime )
{
	MotorControl::setPulseWidth( (uint32_t)_pulseWidthConverter.convertXtoY( degree ), rampTime );
}

int32_t ServoControl::desiredDegree() const
{
	return _pulseWidthConverter.convertYtoX( MotorControl::pulseWidth() );
}


} // namespace IO
} // namespace VehicleControl