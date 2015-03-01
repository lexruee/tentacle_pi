# TSL2561

Raspberry Pi C driver and Python Bindings for the TSL2561 sensor.

This driver is basically a port of the [Adafruit TSL2561 Light Sensor Driver](https://github.com/adafruit/Adafruit_TSL2561).

## Configure I2C

Install the following packages:
* i2c-tools 
* libi2c-dev
* python-dev

```
sudo apt-get install i2c-tools libi2c-dev python-dev
```

In a next step we configure i2c on the Raspberry Pi. 

Add the following lines to `/boot/config.txt`:

```
#i2c
dtparam=i2c_arm=on,i2c1=on
```

Edit the file `/etc/modules` and add the following line:

```
i2c-dev 
```

Finally reboot:

```
sudo reboot
```

After the reboot you should be able to find the i2c device using the command below:

```
sudo i2cdetect -y 1
```

## Building the driver
### C language

Just run `make`.


### Python language

Run `sudo python setup.py build`


## Usage
### C language

```c
#include "tsl2561.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

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

```

### Python language

```python
import TSL2561
import time

tsl = TSL2561.TSL2561(0x39,"/dev/i2c-1")
tsl.enable_autogain()
tsl.set_time(0x00)

for x in range(0,5):
	print "lux %s" % tsl.lux()
	time.sleep(3)

```

## License
Adafruit_TSL2561, a C++ library, is licensed under BSD as specified [here](https://github.com/adafruit/Adafruit_TSL2561); the derivative work tsl2561 is provided under the MIT licence. 
