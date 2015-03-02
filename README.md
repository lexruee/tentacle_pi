# Tentacle Pi

Tentacle Pi is my little open source project for writting C drivers
and Python Bindings for I2C sensors. 


Right know some drivers might be buggy (e.g. AM2315) and there is no
Python 3 support.


Why did I create this repository?

* For my project at the University I need a single repository that
 contains all necessary drivers.
 
* Besides that I want to learn how to write linux drivers for I2C sensors
 and Python C extensions.
 
* I want sensor drivers that are purely written in C for portability 
 and performance reasons.
 
* No additional dependencies like boost, wiringPi or quick2wire.

* Python 2.7 support! Because most of the single-board computers have Python 2.7 support
like Raspberry Pi, Raspberry Pi 2, Banana Pi, or BeagleBone Black.

* No hardcoded platform specific drivers.
 
* My goal is to support further I2C sensors in the future.
 
 
Supported I2C sensors are:

 * AM2315 (buggy)
 * BMP180
 * TSL2561
 
 
## Installation
Install the following packages:

```bash
sudo apt-get install i2c-tools libi2c-dev python-dev
```

Configure I2C on your Raspberry Pi.  On the [Adafruit learning platform](https://learn.adafruit.com/) you will find a great tutorial 
[how to configure I2C](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c).
Besides that their products (AM2315, BMP180, TSL2561) are great :smile_cat: .

### pip installer

```bash
sudo pip install tentacle_pi
```

### from source
Clone this repository:

```bash
git clone https://github.com/lexruee/tentacle_pi
```

Install the python module:

```bash
sudo python setup.py install
```


## Usage

### AM2315

```python

import time
from tentacle_pi.AM2315 import AM2315
am = AM2315(0x5c,"/dev/i2c-1")

for x in range(0,10):
	temperature, humidity, crc_check = am.sense()
	print "temperature: %0.1f" % temperature
	print "humidity: %0.1f" % humidity
	print "crc: %s" % crc_check
	print
	time.sleep(2)


```

### BMP180

```python
from tentacle_pi.BMP180 import BMP180
import time
bmp = BMP180(0x77,"/dev/i2c-1")


for x in range(0,10):
        print "temperature: %0.1f" % bmp.temperature()
        print "pressure: %s" % bmp.pressure()
        print "altitude: %0.1f" % bmp.altitude()
        print
        time.sleep(2)

```


### TSL2561

```python
from tentacle_pi.TSL2561 import TSL2561
import time

tsl = TSL2561(0x39,"/dev/i2c-1")
tsl.enable_autogain()
tsl.set_time(0x00)

for x in range(0,10):
	print "lux %s" % tsl.lux()
	print
	time.sleep(3)

```


## Dependencies

* i2c-tools 
* libi2c-dev
* python-dev
* python 2.7


## Changelog
##### 0.1.0
  * first release
  * add support for i2c sensor BMP180
  * add support for i2c sensor AM2315
  * add support for i2c sensor TSL2561
