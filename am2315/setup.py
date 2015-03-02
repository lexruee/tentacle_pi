from distutils.core import setup, Extension
setup(
	name="tentacle_pi.AM2315", 
	version="1.0",
	packages = ["tentacle_pi"],
	ext_modules = [
		Extension("tentacle_pi.AM2315", 
			sources = ["src/am2315.c", "src/am2315_ext.c"])
	]
)
