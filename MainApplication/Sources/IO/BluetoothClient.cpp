#include "BluetoothClient.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <errno.h>

namespace VehicleControl {
namespace Bluetooth {

Client::Client( const std::string& peerAddress )
	: Base( peerAddress )
	, _isConnected( false )
{

	//	this->_peerAddress->rc_bdaddr;
	//	ba2str( &this->_peerAddress->rc_bdaddr, returnVal );
	//	str2ba( peerAddress.c_str(), &_peerAddress->rc_bdaddr );
	startThread( &this->_setupThread, tryToConnectToServer, &_setupThreadRunning, static_cast< void* >( this ) );

}

Client::~Client()
{

}

bool Client::isConnected() const
{
	return _isConnected;
}

void Client::handleConnectionLoss()
{
	printf("client handled\r\n");
	startThread( &this->_setupThread, tryToConnectToServer, &_setupThreadRunning, static_cast< void* >( this ) );
}

void* Client::tryToConnectToServer( void* input )
{
	Client* client = static_cast< Client* >( input );

	unsigned int returnValue = 0;

	char buff[ 18 ] = { 0 };

	client->_isConnected = false;

	// setup the bluetooth family
	client->_peerAddress->rc_family = AF_BLUETOOTH;

	client->_peerAddress->rc_channel = 1;


	printf("client setup socket %d\r\n", client->_socket );

	ba2str( &client->_peerAddress->rc_bdaddr, buff );

	printf("connecting with:\r\n"
//		   "add:\t%s\r\n"
		   "ch:\t%d\r\n"
		   "fam:\t%d\r\n", client->_peerAddress->rc_channel, client->_peerAddress->rc_family );

	while ( true )
	{
		client->setupSocket();

		returnValue |= connect( client->_socket, (struct sockaddr *)client->_peerAddress, sizeof( *client->_peerAddress ) );

		int errsv = errno;
		printf("client tried to connect %s %i\r\n", strerror( errsv ), errsv );

		if ( errsv )
		{
			sleep( 4 );
		}
		else
		{
			break;
		}
	}

//	int errsv = errno;

//	printf("client tried to connect %s %i\r\n", strerror( errsv ), errsv );

	if ( returnValue )
	{
		client->_isConnected = false;
	}
	else
	{
		client->startBothThreads();
		client->_isConnected = true;
	}

	client->_setupThreadRunning = false;

	//	printf("conn %d\n", client->isConnected() );

	pthread_exit( NULL );
	return 0;
}

} // namespace Bluetooth
} // namespace VehicleControl
//#include <stdio.h>
//#include <stdlib.h>
//#include <signal.h>
//#include <pthread.h>
//#include <unistd.h>
//#include <sys/socket.h>
//#include <bluetooth/bluetooth.h>
//#include <bluetooth/rfcomm.h>

//int s ;
//void ctrl_c_handler(int signal);
//void close_sockets();
//void *readMsg();
//void *sendMsg();

//int main(int argc,char **argv)
//{
//	(void) signal(SIGINT,ctrl_c_handler);

//	pthread_t readT, writeT;
//	char *message1 = "Read thread\n";
//	char *message2 = "Write thread\n";
//	int iret1, iret2;

//	struct sockaddr_rc addr= { 0 };
//	int status ;
//	char dest[18] = "00:1F:81:00:00:01";
//	char msg[25];

//	//allocate a socket
//	s = socket(AF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM);
//	addr.rc_family = AF_BLUETOOTH ;
//	addr.rc_channel = 1 ;
//	str2ba(dest,&addr.rc_bdaddr);

//	//connect to server
//	printf("going 2 connect\n");
//	status = connect(s,(struct sockaddr *)&addr,sizeof(addr)) ;

//	//send a message
//	if(0 == status){
//		printf("connect success\n");

//		/* Create independent threads each of which will execute function */

//		iret1 = pthread_create(&readT,NULL,readMsg,(void*) message1);
//		iret2 = pthread_create(&writeT,NULL,sendMsg,(void*) message2);

//		pthread_join(readT,NULL);
//		pthread_join(writeT,NULL);

//	}


//	close_sockets();
//	return 0;
//}

//void *sendMsg(){
//	char msg[25] ;
//	int status ;

//	do{
//		memset(msg,0,sizeof(msg));
//		fgets(msg,24,stdin);
//		if(strncmp("EXIT",msg,4)==0 || strncmp("exit",msg,4)==0)break;
//		status = send(s,msg,strlen(msg),0);
//		fprintf(stdout,"Status = %d\n",status);
//	}while(status > 0);
//}

//void *readMsg(){
//	int bytes_read;
//	char buf[1024] = { 0 };
//	do{
//		memset(buf,0,sizeof(buf));
//		//read data from the client
//		bytes_read = recv(s,buf,sizeof(buf),0) ;
//		fprintf(stdout,"Bytes read = %d\n",bytes_read);
//		if(bytes_read <= 0)break;
//		fprintf(stdout,"<<>> %s",buf);
//	}while(1);
//}

//void close_sockets(){
//	close(s);
//	fprintf(stdout,"Close sockets\n");
//}

//void ctrl_c_handler(int signal){
//	fprintf(stdout,"Interrupt caught[NO: %d ]\n",signal);
//	close_sockets();
//	exit(0);
//}

