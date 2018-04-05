/*
 * Licensed under the 'LICENSE'.
 * See LICENSE file in the project root for full license information.
 */
#ifndef BLUETOOTHDEFINITIONS_H
#define BLUETOOTHDEFINITIONS_H

#include <string>

#define BeagleBlue

#define BeagleBlueBluetoothAddress ( "80:30:DC:04:39:D4" )
#define Nexus7BluetoothAddress ( "30:85:A9:E0:1F:9A" )
#define BluetoothPollRate ( 50000 ) // in microseconds

static const std::string localAdress( BeagleBlueBluetoothAddress );
static const std::string peerAdress( Nexus7BluetoothAddress );

static const size_t NumberOfBytesPerReceiveMessage = 16;
static const size_t NumberOfBytesPerSendMessage = 4;

#endif // BLUETOOTHDEFINITIONS_H
