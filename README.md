g++ -Wall -Wextra -std=c++17 -O2 src/main.cpp -o computerAudioRecognition.exe 

the above command compiles the main.cpp in src into computerAudioRecognition.exe 

cant figure out how to make the makefile work so the g++ is like, the alternative ig...

you then run .\computerAudioRecognition.exe to see the result

----above it old ? still want to keep it tho


installed CMake and made a path to it in environmental variables
got extentions for cmaketools etc

to avoid using visual studio i needed to link the compiler to cmake tools
so cntrl shift p then select a kit, i had some GCC ... ucrt 64 one which worked

then you configure the cmake so cntl shift p, cmake:configure

then you build the cmake, cmake : build

once i did that i can go into portaudio and mkdir builddir

then  cd builddir 

then cmake -G "MinGW Makefiles" .. 
frankly i looked this bit up, G is to generate, cmake is tool, apparently when i selected the GCC compiler, thats pointing to
GCC in the MinGW environment, this environment is just something that is on the pc, windows ig.
the command then fills the builddir directory with the make file, its just a setup for the makefile that will make compilations

then  cmake --build .

this is the command that actually compiles it, these commands were done in the command line

any changes to main.cpp you can just do Ctrl+Shift+P and the cmake:build, or you can just do cmake --build .
you just have to do the cmake --build . in the build directory in the root, you can run .\computerAudioRecognition.exe from there too