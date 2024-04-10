project("binarypacker")
cmake_minimum_required(VERSION 3.25)
include_directories(./../../_defoldsdk/sdk/include)
add_library (binarypacker MODULE binarypacker/src/binarypacker.cpp binarypacker/src/binarypacker.h binarypacker/src/parsers.cpp binarypacker/src/writers.cpp binarypacker/src/structs.cpp)
