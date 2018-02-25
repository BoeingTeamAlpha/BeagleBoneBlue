#ifndef SERVOLIST_H
#define SERVOLIST_H
namespace VehicleControl {
namespace IO {

struct ServoList
{
    enum Enum
    {
        LeftDriveMotor = 0,
        RightDriveMotor,
        NUM_SERVOS
    };
};

} // namespace IO
} // namespace VehicleControl
#endif // SERVOLIST_H
