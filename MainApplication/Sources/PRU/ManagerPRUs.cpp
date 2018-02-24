#include "ManagerPRUs.h"

#include "PRUDefinitions.h"
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <sys/mman.h>	// mmap
#include <string.h>

#define FILE_ERROR ( -1 )

#include <stdexcept>
#include <stdio.h>

namespace VehicleControl {

ManagerPRUs::ManagerPRUs()
	: _pruRunning( false )
	, _pruSharedMemoryPointer( NULL )

{
	_pruRunning = bindOrUnbindPRU( true ) ? false : true;

	mapVirtualAddressSpace();
}

ManagerPRUs::~ManagerPRUs()
{
	bindOrUnbindPRU( false );
}

ManagerPRUs& ManagerPRUs::instance()
{
	static ManagerPRUs inst;
	return inst;
}

int ManagerPRUs::bindOrUnbindPRU( bool bind )
{
	int fileDescriptor;
	const char* path = bind ? PRU_BIND_PATH : PRU_UNBIND_PATH;

	fileDescriptor = open( path, O_WRONLY );

	if ( fileDescriptor == FILE_ERROR )
	{
		throw std::invalid_argument( "Cannot open PRU path!!!!" );
	}

	if ( access( PRU0_UEVENT, F_OK ) != NULL )
	{
		if ( write( fileDescriptor, PRU0_NAME, PRU_NAME_LEN ) < 0 )
		{
			throw std::invalid_argument( "Cannot access PRU0!!!!" );
		}
	}

	if ( access( PRU1_UEVENT, F_OK ) != NULL )
	{
		if ( write( fileDescriptor, PRU1_NAME, PRU_NAME_LEN ) < 0 )
		{
			throw std::invalid_argument( "Cannot access PRU1!!!!" );
		}
	}

	close( fileDescriptor );

	_pruRunning = bind;

	return 0;
}

int ManagerPRUs::mapVirtualAddressSpace()
{
	uint32_t* pru;
	int memFileDescriptor = open( "/dev/mem", O_RDWR | O_SYNC );

	if ( memFileDescriptor == FILE_ERROR )
	{
		throw std::invalid_argument( "Cannot open /dev/mem!!!!" );
	}

	pru = static_cast< uint32_t* >( mmap( 0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, memFileDescriptor, PRU_ADDR ) );

	if ( pru == MAP_FAILED )
	{
		throw std::invalid_argument( "Cannot map /dev/mem!!!!" );
	}

	close( memFileDescriptor );

	_pruSharedMemoryPointer = pru + PRU_SHAREDMEM / 4;	// Points to start of shared memory

	memset( _pruSharedMemoryPointer, 0, 9 * 4 );

	//////// Remove after testing
	// PRU runs at 200Mhz. find #loops needed
	unsigned int num_loops = ( ( 10000 * 200.0 ) / PRU_SERVO_LOOP_INSTRUCTIONS );
	// write to PRU shared memory
	_pruSharedMemoryPointer[ 1 - 1 ] = num_loops;

	return 0;
}


int ManagerPRUs::restartPRUs()
{
	int returnValue = 0;

	returnValue |= bindOrUnbindPRU( false );

	returnValue |= bindOrUnbindPRU( true );

	return returnValue;
}

int ManagerPRUs::setPRUState( bool on )
{
	if ( _pruRunning == on )
	{
		return -1;
	}

	return bindOrUnbindPRU( on );
}

uint32_t* const ManagerPRUs::sharedMemoryPointer()
{
	return _pruSharedMemoryPointer;
}

const uint32_t* const ManagerPRUs::sharedMemoryPointer() const
{
	return _pruSharedMemoryPointer;
}
} // namespace VehicleControl
