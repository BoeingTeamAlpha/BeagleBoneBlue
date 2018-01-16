#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Input.h"
#include "BoeingSurfaceVehicleControl.h"

#define PRU_NUM 	0

using namespace VehicleControl;
using namespace IO;
int main (void)
{
	if( getuid() != 0 )
	{
		printf("You must run this program as root. Exiting.\n");
		exit( EXIT_FAILURE );
	}

//	printf("This is coool with git!!!!!!!11\r\n");
	unsigned int count = 0;

	Control& control = Control::instance();

	while ( control.isRunning() )
	{
		control.update();
		usleep( 500000 );
	}

	return EXIT_SUCCESS;
}
