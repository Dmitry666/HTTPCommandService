from conans import ConanFile, CMake, tools

class HelloConan(ConanFile):
    name = "HTTPCommandService"
    version = "1.0"
    url = "https://github.com/Dmitry666/HTTPCommandService.git"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": True}
    generators = "cmake"

    def source(self):
        git = tools.Git(folder="HTTPCommandService")
        git.clone("https://github.com/Dmitry666/HTTPCommandService.git", "master")
        tools.replace_in_file("HTTPCommandService/CMakeLists.txt", "include(conan/conanbuildinfo.cmake)", "include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)")
    
    def build_requirements(self):
        self.build_requires("boost/1.76.0")
        self.build_requires("jsoncpp/1.9.4")
        self.build_requires("inih/52")
        
    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="HTTPCommandService")
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("*.h", dst="include", src="install", keep_path=False)
        self.copy("*.lib", dst="libs", src="install", keep_path=False)
        self.copy("*.dll", dst="bins", src="install", keep_path=False)
        self.copy("*.lib", dst="bins", src="install", keep_path=False)
        self.copy("*.pdb", dst="bins", src="install", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["HTTPCommandService"]
        self.cpp_info.libdirs = ['libs']
        self.cpp_info.bindirs = ['bins']