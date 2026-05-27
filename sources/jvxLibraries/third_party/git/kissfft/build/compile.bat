echo off
REM set arch=x64
REM ${archtoken} ${targetfolder} ${dataformattoken}
set arch=%~1
set kissfftdir=%~2
set dataformattoken=%~3
set buildtype=%~4

set generatortoken=-G "Ninja"
del CMakeCache.txt

REM https://stackoverflow.com/questions/7583172/opencv-as-a-static-library-cmake-options
REM https://stackoverflow.com/questions/19772398/build-the-static-opencv-library-with-multi-threaded-debug-dll-mdd-c-runtime-l

REM We need option -DBUILD_opencv_python3=FALSE for x86: Python32 does not exist any longer!
REM build_${archtoken}_${dataformattoken}_${CMAKE_BUILD_TYPE}
echo cmake %generatortoken% -DCMAKE_INSTALL_PREFIX=../kissfft_%arch%_%dataformattoken%_%buildtype% -DKISSFFT_DATATYPE=%dataformattoken% -DKISSFFT_OPENMP=FALSE -DKISSFFT_PKGCONFIG=FALSE -DKISSFFT_STATIC=TRUE -DKISSFFT_TEST=FALSE -DKISSFFT_TOOLS=FALSE -DKISSFFT_USE_ALLOCA=FALSE -DCMAKE_BUILD_TYPE=%buildtype% ../kissfft
cmake %generatortoken% -DCMAKE_INSTALL_PREFIX=../kissfft_%arch%_%dataformattoken%_%buildtype% -DKISSFFT_DATATYPE=%dataformattoken% -DKISSFFT_OPENMP=FALSE -DKISSFFT_PKGCONFIG=FALSE -DKISSFFT_STATIC=TRUE -DKISSFFT_TEST=FALSE -DKISSFFT_TOOLS=FALSE -DKISSFFT_USE_ALLOCA=FALSE -DCMAKE_BUILD_TYPE=%buildtype% ../kissfft

ninja install
ninja clean
