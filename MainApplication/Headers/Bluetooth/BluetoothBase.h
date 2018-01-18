#ifndef BLUETOOTHBASE_H
#define BLUETOOTHBASE_H

#include <pthread.h>
#include <string>

class sockaddr_rc;

namespace VehicleControl {
namespace Bluetooth {

class Base
{
protected:

	// whether the thread should run
	bool _readThreadRunning;
	bool _setupThreadRunning;
	bool _writeThreadRunning;

	int _client;
	int _socket;

	pthread_t _readThread;
	pthread_t _setupThread;
	pthread_t _writeThread;

	sockaddr_rc* _peerAddress;

protected:

	typedef void* ( *StartRoutine )( void * );

	Base( const std::string& peerAddress );

//	sockaddr_rc* clientAddress();

//	sockaddr_rc* localAddress();

	int startBothThreads();

	int startReadThread();

	int startWriteThread();

	int setupSocket();

	static void* readMessage( void* input );

	static void* writeMessage( void* input );

	static int startDetachedThread( pthread_t* thread, StartRoutine startRoutine, bool* isRunningFlag, void* objectPointer );

	virtual void handleConnectionLoss();

public:

	virtual ~Base();

};

} // namespace Bluetooth
} // namespace VehicleControl

#endif // BLUETOOTHBASE_H
