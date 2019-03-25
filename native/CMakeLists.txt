cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

# Default Android build options to follow Godot's settings
# Use $ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake toolchain
if(CMAKE_SYSTEM_NAME STREQUAL Android)
	set(CMAKE_TOOLCHAIN_FILE "$ENV{ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake" CACHE FILEPATH "Android NDK's CMake toolchain file")
	set(ANDROID_PLATFORM android-28 CACHE STRING "Android target platform API level")
	set(ANDROID_ABI arm64-v8a CACHE STRING "Android target ABI architecture")
	set(ANDROID_TOOLCHAIN clang)
	set(ANDROID_STL c++_shared)
endif()

# Settings for Emscripten build
# Use $EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake toolchain
if(CMAKE_SYSTEM_NAME STREQUAL Emscripten)
	set(CMAKE_TOOLCHAIN_FILE "$ENV{EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake" CACHE FILEPATH "Emscripten's CMake toolchain file")
endif()

# Setup additional CMake modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Start GodotCpp library project
project(BattleTanks VERSION 1.0.0 LANGUAGES CXX)

# Setup artifacts' output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

# Force static linking with runtime library on MSVC compiler
if(MSVC)
	string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
	string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endif()

# Find GDNative C++ binding library
find_package(GodotCpp REQUIRED)

# Add thirdparty libraries
add_subdirectory(thirdparty/clipper)
add_subdirectory(thirdparty/polypartition)

# # Add main Battle Tanks's shared library
add_subdirectory(src)
