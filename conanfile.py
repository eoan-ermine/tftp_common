import os

from conan import ConanFile
from conan.tools.files import copy

class TFTPCommonConan(ConanFile):
	name = "tftp_common"
	version = "1.3.0"
	url = "https://github.com/eoan-ermine/tftp_common"
	description = " A simple header-only Trivial File Transfer Protocol packets parsing and serializing library"
	license = "Unlicense"
	exports = ["LICENSE"]
	exports_sources = "tftp_common/*.hpp"
	no_copy_source = True

	def package(self):
		copy(self, "UNLICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
		copy(self, "tftp_common/*.hpp", src=self.source_folder, dst=os.path.join(self.package_folder, "include"))