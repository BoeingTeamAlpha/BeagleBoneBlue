#include "BluetoothBase.h"

#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

namespace VehicleControl {
namespace Bluetooth {

Base::Base( const std::string& peerAddress )
	: _readThreadRunning( false )
	, _writeThreadRunning( false )
	, _client( 0 )
	, _socket( 0 )
{
	_peerAddress = new sockaddr_rc;

	*_peerAddress = { 0 };

	// setup the bluetooth family
	_peerAddress->rc_family = AF_BLUETOOTH;

	// convert the string to the addresses
	str2ba( peerAddress.c_str(), &_peerAddress->rc_bdaddr );

	// setup the port
	_peerAddress->rc_channel = 1;

	printf("based finished const peer add is %s\r\n", peerAddress.c_str() );
}

Base::~Base()
{
	if ( this->_readThreadRunning )
	{
		pthread_cancel( this->_readThread );
	}

	if ( this->_writeThreadRunning )
	{
		pthread_cancel( this->_writeThread );
	}

	if ( this->_setupThreadRunning )
	{
		pthread_cancel( this->_setupThread );
	}

	delete _peerAddress;
}

//sockaddr_rc* Base::clientAddress()
//{
//	return _clientAddress;
//}

//sockaddr_rc* Base::localAddress()
//{
//	return _localAddress;
//}

void* Base::readMessage( void* input )
{
	Base* base = static_cast< Base* >( input );
	int bytes_read;
	char buf[1024] = { 0 };

	while ( base->_readThreadRunning )
	{
		memset( buf, 0, sizeof( buf ) );
		//read data from the client

		bytes_read = recv( base->_client, buf, sizeof( buf ), 0 );

		fprintf( stdout, "Bytes read = %d\n", bytes_read );

		if ( bytes_read <= 0 )
		{
			base->handleConnectionLoss();
			break;
		}

		fprintf(stdout,"<<>> %s",buf);
	}

	base->_readThreadRunning = false;

	printf("Leaving Read Message\r\n");

	close( base->_socket );
	pthread_exit( NULL );
	return 0;
}

void* Base::writeMessage( void* input )
{
	Base* base = static_cast< Base* >( input );

	char msg[25];
	int status;

	while ( base->_writeThreadRunning )
	{
		memset( msg, 0, sizeof( msg ) );
		fgets( msg, 24, stdin );
		if( strncmp("EXIT",msg,4)==0 || strncmp("exit",msg,4) ==0 )
		{
			break;
		}

		status = send( base->_client, msg, strlen( msg ), 0 );
		fprintf( stdout, "Status = %d\n", status );
	}

	base->_writeThreadRunning = false;

	close( base->_client );
	pthread_exit( NULL );
	return 0;
}

int Base::startThread( pthread_t* thread, StartRoutine startRoutine, bool* isRunningFlag, void* objectPointer )
{
	if ( *isRunningFlag )
	{
		return -1;
	}

	int returnValue = 0;

	pthread_attr_t attribute;

	pthread_attr_init( &attribute );

	pthread_attr_setdetachstate( &attribute, PTHREAD_CREATE_DETACHED );

	returnValue |= pthread_create( thread, &attribute, startRoutine, objectPointer );

	pthread_attr_destroy( &attribute );

	if( returnValue )
	{
		perror("Bluetooth base: Failed to create the thread");
		*isRunningFlag = false;
		return -1;
	}
	else
	{
		*isRunningFlag = true;
		return 0;
	}
}

void Base::handleConnectionLoss()
{

}

int Base::startBothThreads()
{
	return startReadThread() | startWriteThread();
}

int Base::startReadThread()
{
	return startThread( &this->_readThread, readMessage, &_readThreadRunning, static_cast< void* >( this ) );
}

int Base::startWriteThread()
{
	return startThread( &this->_writeThread, writeMessage, &_writeThreadRunning, static_cast< void* >( this ) );
}

int Base::setupSocket()
{
//	char buf[18] = { 0 };

//	unsigned int opt = sizeof( *_clientAddress ) ;

	_socket = socket( AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM );
	//allocate socket
	return _socket;
}


} // namespace Bluetooth
} // namespace VehicleControl
