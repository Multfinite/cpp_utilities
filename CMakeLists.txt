# https://github.com/Multfinite/cpp_utilities
cmake_minimum_required (VERSION 3.8)

if(NOT TARGET defs_proxy)
    # https://stackoverflow.com/questions/66554636/cmake-add-compile-definitions-but-for-all-targets
    add_library(defs_proxy INTERFACE)
endif()

add_subdirectory("tiny-process-library")

project("utilities")
message("project: utilities\n-----------------------------------")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
#	add_compile_options(-Og)						# optimization: debug
    add_compile_options(-std=c++20)                             # C++ standard
    add_compile_options(-fpermissive)                             # required for some pieces of code
	add_compile_options(-Wchanges-meaning)
else()
endif()

# test that filesystem header actually is there and works
#try_compile(HAS_FS "${CMAKE_BINARY_DIR}/temp"
#"${CMAKE_CURRENT_SOURCE_DIR}/tests/has_filesystem.cpp"
#            CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=OFF
#            LINK_LIBRARIES stdc++fs)
#try_compile(HAS_FS_EXPERIMENTAL "${CMAKE_BINARY_DIR}/temp"
#"${CMAKE_CURRENT_SOURCE_DIR}/tests/has_experimental_filesystem.cpp"
#            CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=OFF
#            LINK_LIBRARIES stdc++fs)
#
#if(HAS_FS)
#    message(STATUS "Compiler has filesystem support")
#    target_compile_definitions(defs_proxy INTERFACE STD_FS=1)
#elseif(HAS_FS_EXPERIMENTAL)
#    message(STATUS "Compiler has EXPERIMENTAL filesystem support")
#    add_compile_definitions(defs_proxy INTERFACE STD_FS=2)
#else()
#    message(FATAL_ERROR "Compiler is missing filesystem capabilities")
#    add_compile_definitions(defs_proxy INTERFACE STD_FS=0)
#endif()

#file(GLOB_RECURSE src
file(GLOB src "*.cpp")
foreach (isrc ${src})
    message("${isrc}")
endforeach()

add_library (utilities_lib STATIC ${src})

target_link_libraries(utilities_lib PRIVATE defs_proxy)
target_link_libraries(utilities_lib LINK_PUBLIC -Wl,--allow-multiple-definition tiny-process-library -Wl,--allow-multiple-definition)

find_package(date CONFIG)
if(TARGET date OR date_FOUND)
    target_link_libraries(utilities_lib PUBLIC date)
    target_compile_definitions(defs_proxy INTERFACE ENABLE_DATE=1)
else()
    message("date not found, disable related features.")
endif()

find_package(nlohmann_json CONFIG)
if(TARGET nlohmann_json OR nlohmann_json_FOUND)
    target_link_libraries(utilities_lib PUBLIC nlohmann_json::nlohmann_json)
    target_compile_definitions(defs_proxy INTERFACE ENABLE_NLOHMANN_JSON=1)
else()
    message("nlohmann_json not found, disable related features.")
endif()

find_package(stduuid CONFIG)
if(TARGET stduuid OR stduuid_FOUND)
    target_link_libraries(utilities_lib PUBLIC stduuid)
    target_compile_definitions(defs_proxy INTERFACE ENABLE_STDUUID=1)
else()
    message("stduuid not found, disable related features.")
endif()

find_package(spdlog CONFIG)
if(TARGET spdlog OR spdlog_FOUND)
    target_link_libraries(utilities_lib PUBLIC spdlog::spdlog spdlog::spdlog_header_only)
    target_compile_definitions(defs_proxy INTERFACE ENABLE_SPDLOG=1)
else()
    message("spdlog not found, disable related features.")
endif()

#find_package(utf8cpp CONFIG REQUIRED)
#target_link_libraries(utilities_lib PRIVATE utf8cpp)
