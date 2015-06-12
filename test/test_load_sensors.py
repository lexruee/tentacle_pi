#encoding: utf-8
import time
from tentacle_pi.AM2315 import AM2315
from tentacle_pi.BMP180 import BMP180
from tentacle_pi.TSL2561 import TSL2561
from tentacle_pi.MCP9808 import MCP9808
from tentacle_pi.MPL115A2 import MPL115A2
from tentacle_pi.LM75 import LM75

"""
Simple test to check if all supported sensors are working.

- Runs on a Odroid C1 (Ubuntu) and Raspberry Pi 1 / 2 (Raspbian).
- For the Banana Pi (Raspbian) change the variable I2C_DEVICE to '/dev/i2c-2'

"""

I2C_DEVICE = '/dev/i2c-1'


print "test AM2315"
am = AM2315(0x5c, I2C_DEVICE)
temperature, humidity, crc_check = am.sense()
print "temperature: %0.1f" % temperature
print "humidity: %0.1f" % humidity
print "crc: %s" % crc_check
print
time.sleep(2)


print "test BMP180"
bmp = BMP180(0x77, I2C_DEVICE)
print "temperature: %0.1f" % bmp.temperature()
print "pressure: %s" % bmp.pressure()
print "altitude: %0.1f" % bmp.altitude()
print
time.sleep(2)


print "test TSL2561"
tsl = TSL2561(0x39, I2C_DEVICE)
tsl.enable_autogain()
tsl.set_time(0x00)
print "lux %s" % tsl.lux()
print
time.sleep(2)


print "test MCP9808"
mcp = MCP9808(0x18, I2C_DEVICE)
temperature = mcp.temperature()
print "temperature: %0.2f" % temperature
print
time.sleep(2)


print "test MPL115A2"
mpl = MPL115A2(0x60, I2C_DEVICE)
temperature, pressure = mpl.temperature(), mpl.pressure()
print "temperature: %0.1f" % temperature
print "pressure: %0.1f" % pressure
print
time.sleep(2)


print "test LM75"
lm = LM75(0x48, I2C_DEVICE)
temperature = lm.temperature()
print "temperature: %0.2f" % temperature
time.sleep(2)
