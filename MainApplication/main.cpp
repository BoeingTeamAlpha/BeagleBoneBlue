#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Input.h"
#include "BoeingSurfaceVehicleControl.h"
#include "ManagerPRUs.h"
#define PRU_NUM 	0

using namespace VehicleControl;
using namespace LibBBB::IO;

int main()
{
	if( getuid() != 0 )
	{
		printf("You must run this program as root. Exiting.\n");
		exit( EXIT_FAILURE );
	}

//	printf("This is coool with git!!!!!!!11\r\n");
//	unsigned int count = 0;

//	ManagerPRUs& pruManager = ManagerPRUs::instance();
	/**
	 * TODO: a better approach is to put this in vehicle control's constructor
	 * and stop the bluetooth threads from spawning until AFTER control
	 * has been constructed. This will allow a sleep to be put in after
	 * manager::instance to allow Linux to catch up after we set up
	 * the OCP port mem map.
	 */
	LibBBB::ManagerPRUs::instance();

	usleep( 500000 );
	Control& control = Control::instance();

	while ( control.isRunning() )
	{
		control.update();
		usleep( BluetoothPollRate );
	}

	return EXIT_SUCCESS;
}
