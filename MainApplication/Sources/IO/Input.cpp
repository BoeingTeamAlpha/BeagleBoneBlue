#include "Input.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>

using namespace std;
namespace VehicleControl {
namespace IO {

Input::Input( const Setup& setup )
	: IOBase( setup.number, Direction::Input )
	, _debounceTime( setup.debounceTime )
{
	this->_callbackFunction = setup.callback;
	this->setEdgeType( setup.desiredEdge );
	IOBase::setActiveHighOrLow( setup.isActiveLow );
	if ( this->_callbackFunction )
	{
		this->waitForEdgeThreaded();
	}
}

Input::~Input()
{
	pthread_cancel( this->_thread );
}

void Input::setDebounceTime( int time )
{
	this->_debounceTime = time;
}

int Input::getDebounceTime() const
{
	return this->_debounceTime;
}

int Input::setEdgeType( Input::Edge::Enum edge )
{
	switch( edge )
	{
	case Edge::NONE:
		return IOBase::write( "edge", "none" );
		break;

	case Edge::RISING:
		return IOBase::write( "edge", "rising" );
		break;

	case Edge::FALLING:
		return IOBase::write( "edge", "falling" );
		break;

	case Edge::BOTH:
		return IOBase::write( "edge", "both" );
		break;
	}

	return -1;
}

void Input::setEdgeCallback( IOBase::CallbackType callback /* = NULL */ )
{
	this->_callbackFunction = callback;
}

Input::Edge::Enum Input::getEdgeType()
{
	string input = IOBase::read( "edge" );
	if ( input == "rising" )
	{
		return Edge::RISING;
	}
	else if ( input == "falling" )
	{
		return Edge::FALLING;
	}
	else if ( input == "both" )
	{
		return Edge::BOTH;
	}
	else
	{
		return Edge::NONE;
	}
}

#define LoopErrorValue ( 2 )
int Input::waitForEdge()
{
	int fd, i, epollfd, count = 0;
	struct epoll_event ev;

	// create the epoll
	epollfd = epoll_create( 1 );

	// check to make sure the epoll was created successfully
	if ( epollfd == -1 )
	{
		perror("GPIO: Failed to create epollfd");
		return -1;
	}

	// open the file descriptor
	if ( ( fd = open( ( this->path() + "value" ).c_str(), O_RDONLY | O_NONBLOCK ) ) == -1 )
	{
		perror("GPIO: Failed to open file");
		return -1;
	}

	// set the events:
	//			read operation | edge triggered | urgent data
	ev.events = EPOLLIN | EPOLLET | EPOLLPRI;

	// attach the file file descriptor
	ev.data.fd = fd;

	//Register the file descriptor on the epoll instance, see: man epoll_ctl
	if ( epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &ev ) == -1 )
	{
		perror( "GPIO: Failed to add control interface" );
		return -1;
	}

	// loop to allow the epoll to run twice
	while( count <= 1 )
	{
		// wait for the epoll. The 1 means one event, and the -1 means wait for infinity
		// for the event
		i = epoll_wait( epollfd, &ev, 1, -1 );

		// if there is an error in epoll_wait
		if ( i == -1 )
		{
			// print error and leave the loop
			perror( "GPIO: Poll Wait fail" );
			count = LoopErrorValue;
		}
		else // no error in epoll_wait
		{
			// increment the count
			count++;
		}
	}

	// close the file descriptor
	close( fd );

	// return -1 if there is an error
	if ( count == LoopErrorValue )
	{
		return -1;
	}

	return 0;
}

int Input::waitForEdgeThreaded()
{
	// set the flag to true
	this->_threadRunning = true;

	// create an attribute and init it
	pthread_attr_t attribute;
	pthread_attr_init( &attribute );

	// set the thread to be detached, since the calling thread does not need to wait for this
	// thread to join
	pthread_attr_setdetachstate( &attribute, PTHREAD_CREATE_DETACHED );

	// create the thread
	int returnValue = pthread_create( &this->_thread, &attribute, &threadedPoll, static_cast< void* >( this ) );

	// destroy the attribute
	pthread_attr_destroy( &attribute );

	// if the pthread was not created successfully, exit the thread
	if ( returnValue )
	{
		perror("GPIO: Failed to create the poll thread");
		this->_threadRunning = false;
		return -1;
	}
	else // thread successfully created
	{
		// return no error
		return 0;
	}
}

void Input::cancelWaitForEdge()
{
	this->_threadRunning = false;
}

void* Input::threadedPoll( void* value )
{
	// cast the void* to a pointer to this class
	Input* input = static_cast< Input* >( value );

	// loop until the thread stops running
	while ( input->_threadRunning )
	{
		// call the wait for edge method
		input->_callbackFunction( input->waitForEdge() );

		// sleep for the debounce time ( * 1000 just turns milliseconds to microseconds )
		usleep( input->_debounceTime * 1000 );
	}

	// safely exit the thread
	pthread_exit( NULL );

	// return NULL
	return NULL;
}

} // namespace IO
} // namespace VehicleControl
