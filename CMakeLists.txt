# https://github.com/Multfinite/cpp_utilities
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "/utf-8")
set(CMAKE_C_FLAGS "/utf-8")

message("project: utilities")
message("CXX `${CMAKE_CXX_STANDARD}`")
message("CXX_FLAGS `${CMAKE_CXX_FLAGS}`")
message("C_FLAGS `${CMAKE_C_FLAGS}`")
message("CXX_COMPILER_ID `${CMAKE_CXX_COMPILER_ID}`")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_compile_options(/bigobj)
	add_compile_options(/std:c++20)
	add_link_options(/FORCE:MULTIPLE)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_compile_options(-g)						# debug info, system specific
	add_compile_options(-Og)						# optimization: debug
	add_compile_options(-std=c++20)			# C++ standard
	add_compile_options(-fpermissive)			# required for some pieces of code
	add_compile_options(-Wchanges-meaning)
else()
endif()

#file(GLOB_RECURSE src
file(GLOB src
	"*.cpp",
)
foreach (isrc ${src})
    message("${isrc}")
endforeach()

add_library (utilities_lib STATIC ${src})
add_subdirectory("tiny-process-library")

target_link_libraries (utilities_lib LINK_PUBLIC -Wl,--allow-multiple-definition tiny-process-library -Wl,--allow-multiple-definition)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(utilities_lib PRIVATE nlohmann_json::nlohmann_json)

find_package(stduuid CONFIG REQUIRED)
target_link_libraries(utilities_lib PRIVATE stduuid)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(utilities_lib PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

#find_package(utf8cpp CONFIG REQUIRED)
#target_link_libraries(utilities_lib PRIVATE utf8cpp)

message("project: utilities - done")
