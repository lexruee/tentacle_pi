#include "am2315.h"
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv){
	char *i2c_device = "/dev/i2c-1";
	int address = 0x5c;
	
	void *am = am2315_init(address, i2c_device);
	
	
	if(am != NULL){
		float temperature, humidity;
		int i;
		for(i = 0; i < 10; i++) {
			printf("-------- %i ---------\n", i);
			usleep(2*1000*1000);
			am2315_read_data(am, &temperature, &humidity);
			printf("temperature = %f, humidity = %f\n", temperature, humidity);
		
		}
		am2315_close(am);
	}
	
	return 0;
}
