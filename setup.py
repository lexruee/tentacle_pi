#encoding: utf-8
from distutils.core import setup, Extension
import os

DRIVERS = [
	# (module_name, module_dir, module_src_dir)
	("tentcale_pi.AM2315","am2315","src"),
	("tentcale_pi.BMP180", "bmp180", "src"),
	("tentcale_pi.TSL2561", "tsl2561", "src")
]

CURRENT_DIR = os.path.dirname(os.path.realpath(__file__))

def make_extensions(drivers):
	exts = []
	for (extension_name, extension_dir, extension_src_dir) in drivers:
		ext_path = os.path.join(CURRENT_DIR, extension_dir)
		src_path = os.path.join(ext_path, extension_src_dir)
		c_files = filter(lambda x: x.endswith(".c"), src_path)
		c_files = [ os.path.join(CURRENT_DIR, f) for f in c_files ]
		exts.append((extension_name, c_files, src_path))
	return exts
	
	
def make_manifest_file(extensions):
	with open(os.path.join(CURRENT_DIR,"MANIFEST.in"),"w+") as f:
		[ f.write("graft %s\n" % src_path) for (_,_,src_path) in extensions]
	

extensions = make_extensions(DRIVERS)

make_manifest_file(extensions)

ext_modules = [ Extension(name, src_dir) for (name, src_dir,_) in extensions ] 

setup(
	name = "tentacle_pi",
	version = "0.1.0.dev1",
	description = "Tentacles for the Raspberry Pi!",
	
	ext_modules = ext_modules,
	
	author = "Alexander Rüedlinger",
	author_email = "a.rueedlinger@gmail.com",
	license = "MIT"
)
