cmake_minimum_required(VERSION 3.20)
project(Win32Api C)

set(CMAKE_C_STANDARD 99)

set(GLOBAL global.h)
set(ARRAY array/array.h array/array.c)
set(BUFFER buffer/buffer.h)
set(DATAMODEL dataModel/dataModel.h dataModel/dataModel.c)

add_executable(Win32Api ${GLOBAL} main.c ${ARRAY} ${BUFFER} ${DATAMODEL} BufferedRead.h BufferedRead.c)
