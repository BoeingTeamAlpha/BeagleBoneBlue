#ifndef TEST_H
#define TEST_H

#include <pthread.h>
#include <string>
//#include <bluetooth/rfcomm.h>

namespace VehicleControl {
namespace Bluetooth {

class Test
{
private:

	typedef void* ( *StartRoutine )( void * );
//	typedef sockaddr_rc SocketAddress;
private:

	// whether the thread should run
	bool _readThreadRunning;
	bool _writeThreadRunning;

	int _client;

	int _socket;

	pthread_t _readThread;
	pthread_t _writeThread;

//	sockaddr_rc _clientAddress;
//	sockaddr_rc _localAddress;

private:

	int startBothThreads();

	int startReadThread();

	int startWriteThread();

	int setupSocket();

public:

	Test( const std::string& clientAddress, const std::string& localAddress );

	virtual ~Test();

private:

	static void* readMessage( void* input );
	static void* writeMessage( void* input );

	static int startThread( pthread_t* thread, StartRoutine startRoutine, bool* isRunningFlag, void* objectPointer );
};

} // namespace Bluetooth
} // namespace VehicleControl

#endif // TEST_H
