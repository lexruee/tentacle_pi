import TSL2561
import time

tsl = TSL2561.TSL2561(0x39,"/dev/i2c-1")
tsl.enable_autogain()
tsl.set_time(0x00)

for x in range(0,100):

        print "lux %s" % tsl.lux()
        time.sleep(0.1)
