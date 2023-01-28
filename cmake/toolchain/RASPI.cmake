#

message(" -- CMake Linux for Raspi toolchain -- ")
# Standard settings
set (CMAKE_SYSTEM_NAME Linux)
set (CMAKE_SYSTEM_VERSION 1)
set (UNIX True)
set (JVX_CROSS_COMPILE True)

# specify the cross compiler
# JVX_LINUX_CROSSCOMPILER_ROOT -> /path/to/x-tools/toolchain
# JVX_LINUX_CROSSCOMPILER_LIB_ROOT -> /path/to/entry/raspberry/fs
# JVX_CROSSCOMPILE_PREFIX -> arm-linux-gnueabihf

# Set paths to compilers
SET(CMAKE_C_COMPILER /home/hauke/develop/x-tools/arm-unknown-linux-gnueabihf/bin/arm-unknown-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER /home/hauke/develop/x-tools/arm-unknown-linux-gnueabihf/bin/arm-unknown-linux-gnueabihf-g++)
SET(JVX_CROSSCOMPILE_PREFIX arm-linux-gnueabihf)

SET(CMAKE_LIBRARY_PATH "/usr/lib/${JVX_CROSSCOMPILE_PREFIX}")
SET(CMAKE_PREFIX_PATH  "/usr/lib/${JVX_CROSSCOMPILE_PREFIX}/cmake")

# only search the iOS sdks, not the remainder of the host filesystem
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)




#SET(CMAKE_FIND_ROOT_PATH ${JVX_LINUX_CROSSCOMPILER_ROOT} ${JVX_LINUX_CROSSCOMPILER_LIB_ROOT})
#set(CMAKE_SYSROOT ${JVX_LINUX_CROSSCOMPILER_LIB_ROOT})
