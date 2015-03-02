# AM2315

Raspberry Pi C driver and Python bindings for the sensor AM2315.


## Example Usage

### C language

```c
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

```

### Python language


```python
import time
from tentacle_pi.AM2315 import AM2315
am = AM2315(0x5c,"/dev/i2c-1")

for x in range(0,10):
	temperature, humidity, crc_check = am.sense()
	print "temperature: %s" % temperature
	print "humidity: %s" % humidity
	print "crc: %s" % crc_check
	time.sleep(2)


```

## Dependencies

* i2c-tools 
* libi2c-dev
* python-dev
