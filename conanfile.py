from conans import ConanFile, CMake, tools, errors
from conans.client.conan_api import ConanAPIV1

class FPSGymConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "Catch2/2.7.2@catchorg/stable", "spdlog/1.3.1@bincrafters/stable", "fmt/5.3.0@bincrafters/stable"
    generators = "cmake"

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()