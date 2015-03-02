from distutils.core import setup, Extension
setup(
	name="tentacle_pi.TSL2561", 
	version="1.0",
	packages = ["tentacle_pi"],
	ext_modules = [
		Extension("tentacle_pi.TSL2561", 
			sources = ["src/tsl2561.c", "src/tsl2561_ext.c"])
	]
)
