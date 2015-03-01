from distutils.core import setup, Extension
setup(
	name="BMP180", 
	version="1.0",
	ext_modules = [
		Extension("BMP180", 
			sources = ["src/bmp180.c", "src/bmp180_ext.c"])
	]
)
