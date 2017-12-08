SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm)

#SET(COMPILER_ROOT /home/plasma/tizen-studio/tools/arm-linux-gnueabi-gcc-4.9)
#set(COMPILER_ROOT /usr/arm-linux-gnueabi)
set(COMPILER_ROOT /home/plasma/dlib/dlib/cmake_toolchain/gcc-linaro-5.1-2015.08-x86_64_arm-linux-gnueabi)

#SET(CMAKE_C_COMPILER ${COMPILER_ROOT}/bin/arm-linux-gnueabi-gcc-4.9.2)
#SET(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabi-gcc-5)   
#SET(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabi-g++-5)

set(CMAKE_C_COMPILER ${COMPILER_ROOT}/bin/arm-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER ${COMPILER_ROOT}/bin/arm-linux-gnueabi-g++)

SET(CMAKE_EXE_LINKER_FLAGS "-W1,--no-as-needed -std=c++11 -pthread")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread -fPIC")

set(CMAKE_FIND_ROOT_PATH ${COMPILER_ROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
