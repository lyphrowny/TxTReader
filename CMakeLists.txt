cmake_minimum_required(VERSION 3.20)
project(Win32Api C)

set(CMAKE_C_STANDARD 99)

set(GLOBAL global.h global.c)
set(ARRAY array/array.h array/array.c)
set(DATAMODEL dataModel/dataModel.h dataModel/dataModel.c)
set(VIEWMODEL viewModel/viewModel.h viewModel/viewModel.c)

add_executable(Win32Api ${GLOBAL} main.c ${ARRAY} ${DATAMODEL} ${VIEWMODEL} BufferedRead.h BufferedRead.c)
