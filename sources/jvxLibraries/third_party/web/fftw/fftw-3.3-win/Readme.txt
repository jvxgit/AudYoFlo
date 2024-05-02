FFTW Library taken from here:

http://fftw.org/

UPDATE: You can build fftw also from the source code: download the tar.gz, unpack it and then, run

cmake -DCMAKE_BUILD_TYPE=Debug ..\fftw-3.3.10  -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=FALS

(Example for Windows)



