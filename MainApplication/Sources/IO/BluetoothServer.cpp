#include "BluetoothServer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

namespace VehicleControl {
namespace Bluetooth {

Server::Server( const std::string& peerAddress, const std::string& localAddress )
	: Base( peerAddress )
	, _isConnected( false )
{
	_localAddress = new sockaddr_rc;

	*_localAddress = { 0 };

	// setup the bluetooth family
	_localAddress->rc_family = AF_BLUETOOTH;

	// setup the port
	_localAddress->rc_channel = 1;

	// convert the string to the addresses
	str2ba( localAddress.c_str(), &_localAddress->rc_bdaddr );

	startThread( &this->_setupThread, waitForConnectionsConnections, &_setupThreadRunning, static_cast< void* >( this ) );
//	printf("finished server const with local of %s\r\n", localAddress.c_str() );
}

Server::~Server()
{
	delete _localAddress;
}

bool Server::isConnected() const
{
	return _isConnected;
}

void Server::handleConnectionLoss()
{
	printf("server handled\r\n");
	startThread( &this->_setupThread, waitForConnectionsConnections, &_setupThreadRunning, static_cast< void* >( this ) );
}

void* Server::waitForConnectionsConnections( void* input )
{
	Server* server = static_cast< Server* >( input );
	unsigned int opt = sizeof( *server->_peerAddress ) ;

	unsigned int returnValue = 0;

	server->_isConnected = false;

	server->setupSocket();

	returnValue |= bind( server->_socket, (struct sockaddr *)server->_localAddress, sizeof( *server->_localAddress ) );
	printf("Binding success %d\n", returnValue );

	//put socket into listen mode
	returnValue |= listen( server->_socket, 1 ) ;

	printf("socket in listen mode %d\n", returnValue );

	server->_client = accept( server->_socket, (struct sockaddr *)server->_peerAddress, &opt );

	int errsv = errno;

	fprintf( stdout, "Connection accepted from %s\r\n", strerror( errsv ) );

	if ( returnValue )
	{
		server->_isConnected = false;
	}
	else
	{
		server->startBothThreads();
		server->_isConnected = true;
	}

	server->_setupThreadRunning = false;

	printf("conn %d\n", server->isConnected() );

	pthread_exit( NULL );
	return 0;
}

} // namespace Bluetooth
} // namespace VehicleControl
