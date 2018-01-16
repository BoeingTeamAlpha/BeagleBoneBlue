#ifndef BLUETOOTHDEFINITIONS_H
#define BLUETOOTHDEFINITIONS_H

#include <string>

#define BeagleBlue
//#define BeagleBlack
//#define Nexus7

#define BeagleBlueBluetoothAddress ( "80:30:DC:04:39:D4" )
#define BeagleBlackBluetoothAddress ( "80:30:DC:E9:C5:92" )
#define Nexus7BluetoothAddress ( "30:85:A9:E0:1F:9A" )

#if defined( BeagleBlue )
	static const std::string localAdress( BeagleBlueBluetoothAddress );
	static const std::string peerAdress( Nexus7BluetoothAddress );
#elif defined( BeagleBlack )
	static const std::string localAdress( BeagleBlackBluetoothAddress );
	static const std::string peerAdress( BeagleBlueBluetoothAddress );
#endif

#endif // BLUETOOTHDEFINITIONS_H
