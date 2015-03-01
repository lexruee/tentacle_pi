#include "tsl2561.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/*
 * Example
 */
int main(int argc, char **argv) {
	int address = 0x39;
	char *i2c_device = "/dev/i2c-1";

	void *tsl = tsl2561_init(address, i2c_device);
	tsl2561_enable_autogain(tsl);
	tsl2561_set_integration_time(tsl, TSL2561_INTEGRATION_TIME_13MS);
 
	if(tsl == NULL){ // check if error is present
		exit(1);
	} 
	
	int c;
	long lux;
	for(c = 0; c < 5; c++){
		lux = tsl2561_lux(tsl);
		printf("lux %lu\n", lux);
		usleep(3 * 1000 * 1000);
	}
	
	tsl2561_close(tsl);
	i2c_device = NULL;
	return 0;
}
