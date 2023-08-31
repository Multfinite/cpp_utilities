cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("project: utilities")
message("COMPILIER IS `${CMAKE_CXX_COMPILER_ID}`")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_compile_options(/bigobj)
	add_link_options(/FORCE:MULTIPLE)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	#if(WIN32)
	#	add_compile_options(-march=x86-64)
	#endif()
	#add_compile_options(-m64)					# 
	#add_compile_options(-g)						# debug info, system specific
	add_compile_options(-Og)						# optimization: debug
	add_compile_options(-std=c++17)			# C++ standard
	add_compile_options(-fpermissive)			# required for some pieces of code
	add_compile_options(-Wchanges-meaning)
	#add_compile_options(-Wa,-mbig-obj)	# File with no section limit (fix assembler: file too big)
else()
endif()

file(GLOB src
	"*.cpp"
)

add_library (utilities_lib STATIC ${src})
add_subdirectory("tiny-process-library")

target_link_libraries (utilities_lib LINK_PUBLIC -Wl,--allow-multiple-definition tiny-process-library -Wl,--allow-multiple-definition)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(utilities_lib PRIVATE nlohmann_json::nlohmann_json)

find_package(stduuid CONFIG REQUIRED)
target_link_libraries(utilities_lib PRIVATE stduuid)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(utilities_lib PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

message("project: utilities - done")