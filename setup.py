#encoding: utf-8
from distutils.core import setup, Extension
from distutils.command import sdist as sdist_module
import os

DRIVERS = [
	# (module_name, module_dir, module_src_dir)
	("tentacle_pi.AM2315","tentacle_pi/am2315","src"),
	("tentacle_pi.BMP180", "tentacle_pi/bmp180", "src"),
	("tentacle_pi.TSL2561", "tentacle_pi/tsl2561", "src"),
	("tentacle_pi.MCP9808", "tentacle_pi/mcp9808", "src"),
	("tentacle_pi.MPL115A2", "tentacle_pi/mpl115a2", "src"),
	("tentacle_pi.LM75", "tentacle_pi/lm75", "src")
]

CURRENT_DIR = os.path.dirname(os.path.realpath(__file__))

def make_extensions(drivers):
	exts = []
	for (extension_name, extension_dir, extension_src_dir) in drivers:
		src_path = os.path.join(extension_dir, extension_src_dir)
		c_files = os.listdir(src_path)
		c_files = filter(lambda x: x.endswith(".c"), c_files)
		c_files = [ os.path.join(src_path, f) for f in c_files ]
		exts.append((extension_name, c_files, src_path))
	return exts


def make_manifest_file(extensions):
	with open(os.path.join(CURRENT_DIR,"MANIFEST.in"),"w+") as f:
		[ f.write("graft %s\n" % src_path) for (_,_,src_path) in extensions]



class sdist(sdist_module.sdist):

	def run(self):
		extensions = make_extensions(DRIVERS)
		make_manifest_file(extensions)
		sdist_module.sdist.run(self)

setup(
	name = "tentacle_pi",
	version = "0.6.0",
	description = "Tentacles for the Raspberry Pi!",
	url = "https://github.com/lexruee/tentacle_pi",
	author = "Alexander Rüedlinger",
	author_email = "a.rueedlinger@gmail.com",
	packages = ["tentacle_pi"],

	cmdclass = {
		"sdist": sdist,
	},

	ext_modules = [ Extension(name, src_dir) for (name, src_dir,_) in make_extensions(DRIVERS) ],

	license = "MIT"
)
