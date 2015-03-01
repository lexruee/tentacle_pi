from distutils.core import setup, Extension
setup(
	name="TSL2561", 
	version="1.0",
	ext_modules = [
		Extension("TSL2561", 
			sources = ["src/tsl2561.c", "src/tsl2561_ext.c"])
	]
)
