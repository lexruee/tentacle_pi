#encoding: utf-8
errors = []
try:
    from tentacle_pi.AM2315 import AM2315
    am = AM2315(0x5c,"/dev/i2c-1")
except RuntimeError as e:
    errors.append(['AM2315', e])

try:
    from tentacle_pi.BMP180 import BMP180
    bmp = BMP180(0x77,"/dev/i2c-1")
except RuntimeError as e:
    errors.append(['BMP180', e])

try:
    from tentacle_pi.TSL2561 import TSL2561
    tsl = TSL2561(0x39,"/dev/i2c-1")
except RuntimeError as e:
    errors.append(['TSL2561', e])

try:
    from tentacle_pi.MCP9808 import MCP9808
    mcp = MCP9808(0x18,"/dev/i2c-1")
except RuntimeError as e:
    errors.append(['MCP9808', e])

try:
    from tentacle_pi.MPL115A2 import MPL115A2
    mpl = MPL115A2(0x60,"/dev/i2c-1")
except RuntimeError as e:
    errors.append(['MPL115A2', e])

try:
    from tentacle_pi.LM75 import LM75
    lm = LM75(0x48,"/dev/i2c-1")
except RuntimeError as e:
    errors.append(['LM75', e])

for driver, error in errors:
    print "driver: %s - %s" % (driver, error)
