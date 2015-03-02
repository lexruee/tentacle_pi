#encoding: utf-8
from distutils.core import setup, Extension
import os

drivers = [
	# (module_name, module_dir, module_src_dir)
	("tentcale_pi.AM2315","am2315","src"),
	("tentcale_pi.BMP180", "bmp180", "src"),
	("tentcale_pi.TSL2561", "tsl2561", "src")
]

def make_extensions(drivers):
	exts = []
	for (extension_name, extension_dir, extension_src_dir) in drivers:
		current_dir = os.path.realpath(__file__)
		ext_path = os.path.join(current_dir, extension_dir)
		src_path = os.path.join(ext_path, extension_src_dir)
		c_files = filter(lambda x: x.endswith(".c"), src_path)
		c_files = map(lambda f: os.path.join(current_dir, f), c_files)
		exts.append((extension_name, c_files, src_path))
	return exts
	
	
def make_manifest_file(extensions):
	with open("./MANIFEST.in","w+") as f:
		[ f.write("graft %s\n" % src_path) for (_,_,src_path) in extensions]
	

extensions = make_extensions(drivers)

make_manifest_file(extensions)

ext_modules = map(lambda (name, src_dir,_): Extension(name, src_dir), extensions) 

setup(
	name = "tentacle_pi",
	version = "0.1.0.dev1",
	description = "Tentacles for the Raspberry Pi!",
	
	ext_modules = ext_modules,
	
	author = "Alexander Rüedlinger",
	author_email = "a.rueedlinger@gmail.com",
	license = "MIT"
)
