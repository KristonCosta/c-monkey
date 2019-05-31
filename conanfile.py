from conans import ConanFile, CMake

class FPSGymConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "Catch2/2.7.2@catchorg/stable", "spdlog/1.3.1@bincrafters/stable", "fmt/5.3.0@bincrafters/stable"
    generators = "cmake"
    options = {"with_coverage": [True, False]}
    default_options = {
      "fmt:header_only": True,
      "with_coverage": False
    }


    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        cmake = CMake(self)
        cmake.configure(defs={"CODE_COVERAGE":"ON", "CMAKE_BUILD_TYPE":"Debug"})
        cmake.build()
        cmake.test()