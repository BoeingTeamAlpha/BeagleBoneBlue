#ifndef BLUETOOTHCLIENT_H
#define BLUETOOTHCLIENT_H

#include "BluetoothBase.h"

namespace VehicleControl {
namespace Bluetooth {

class Client : public Base
{
private:

	bool _isConnected;
public:

	Client( const std::string& peerAddress );

	~Client();

	bool isConnected() const;

private:

	void handleConnectionLoss();
	static void* tryToConnectToServer( void* input );
};

} // namespace Bluetooth
} // namespace VehicleControl
#endif // BLUETOOTHCLIENT_H
