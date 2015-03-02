from tentacle_pi.BMP180 import BMP180
import time
bmp = BMP180(0x77,"/dev/i2c-1")


for x in range(0,5):
        print "temperature: %s" % bmp.temperature()
        print "pressure: %s" % bmp.pressure()
        print "altitude: %s" % bmp.altitude()
        print
        time.sleep(2)

