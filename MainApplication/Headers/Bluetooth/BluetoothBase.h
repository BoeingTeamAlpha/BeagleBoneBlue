/*
 * Licensed under the 'LICENSE'.
 * See LICENSE file in the project root for full license information.
 */
#ifndef BLUETOOTHBASE_H
#define BLUETOOTHBASE_H

#include <pthread.h>
#include <string>

class sockaddr_rc;

namespace VehicleControl {
namespace Bluetooth {

/**
 * @brief	The Base class is the base class for the bluetooth client and server.
 *			This class contains helper methods to start threads, client address,
 *			and the socket. This class will take the string version of the client
 *			address, and turn it into the bluetooth address used in RFCOMM.
 * @todo	GET BLUETOOTH WORKING!!!!
 */
class Base
{
protected:

	// whether each thread should run
	bool _readThreadRunning;
	bool _setupThreadRunning;
	bool _writeThreadRunning;

	// FD for the client and socket
	int _client;
	int _socket;

	// pthread instances
	pthread_t _readThread;
	pthread_t _setupThread;
	pthread_t _writeThread;

	// pointer to the peer address
	sockaddr_rc* _peerAddress;

protected:

	/**
	 * Typedef of the prototype necessary for pthread functions.
	 */
	typedef void* ( *StartRoutine )( void * );

	/**
	 * @brief Base constructor
	 * @param peerAddress address of the device that this code is connected.
	 */
	Base( const std::string& peerAddress );

//	sockaddr_rc* clientAddress();

//	sockaddr_rc* localAddress();

	/**
	 * @brief startBothThreads
	 * @return
	 */
	int startBothThreads();

	/**
	 * @brief startReadThread
	 * @return
	 */
	int startReadThread();

	/**
	 * @brief startWriteThread
	 * @return
	 */
	int startWriteThread();

	/**
	 * @brief setupSocket
	 * @return
	 */
	int setupSocket();

	/**
	 * @brief readMessage
	 * @param input
	 * @return
	 */
	static void* readMessage( void* input );

	/**
	 * @brief writeMessage
	 * @param input
	 * @return
	 */
	static void* writeMessage( void* input );

	/**
	 * @brief startDetachedThread
	 * @param thread
	 * @param startRoutine
	 * @param isRunningFlag
	 * @param objectPointer
	 * @return
	 */
	static int startDetachedThread( pthread_t* thread, StartRoutine startRoutine, bool* isRunningFlag, void* objectPointer );

	/**
	 * @brief handleConnectionLoss
	 */
	virtual void handleConnectionLoss();

public:

	/**
	 * @brief ~Base
	 */
	virtual ~Base();
};

} // namespace Bluetooth
} // namespace VehicleControl

#endif // BLUETOOTHBASE_H
