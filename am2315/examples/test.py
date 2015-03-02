import time
from tentacle_pi.AM2315 import AM2315
am = AM2315(0x5c,"/dev/i2c-1")

for x in range(0,100):
	temperature, humidity, crc_check = am.sense()
	print "temperature: %0.1f" % temperature
	print "humidity: %0.1f" % humidity
	print "crc_ok: %s" % (crc_check == 1)
	time.sleep(2)

