@ECHO OFF
Set CONAN_REVISIONS_ENABLED=1
set CONAN_DISABLE_CHECK_COMPILER

conan install . -if=.\conan -s os=Windows -s arch=x86_64 -s build_type=Release -s compiler="Visual Studio" -s compiler.version=15
mkdir build & cd build
cmake -G "Visual Studio 15 2017" -A x64 -S .. -B "x86_64"
cmake --build "x86_64" --config Release
cmake --install "x86_64"
@pause
