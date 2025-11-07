cmake_minimum_required(VERSION 4.0.0)

project(GVM CXX)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-O3)

set(LIB_HEADER_FILES
	./genlib.hpp
)
set(LIB_SOURCE_FILES
    ./main.cpp
)

set(GVM_HEADER_FILES
	./src/object.hpp
    ./src/instructions.hpp
    ./src/stack_frame.hpp
    ./src/vm.hpp
)
set(GVM_SOURCE_FILES
    ./src/vm.cpp
    ./src/object.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(lib ${LIB_HEADER_FILES} ${LIB_SOURCE_FILES})
add_executable(gvm ${GVM_HEADER_FILES} ${GVM_SOURCE_FILES})

target_link_libraries(lib)
target_link_libraries(gvm)