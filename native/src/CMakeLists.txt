# Battle Tanks main runtime library
# Emitted targets: BattleTanksLibrary

# Create game's main shared library
add_library(BattleTanksLibrary SHARED
	ChunkBody2D.cpp
	ChunkBody2D.hpp
	DrawCache.cpp
	DrawCache.hpp
	main.cpp
)

# Configure the library target
target_include_directories(BattleTanksLibrary PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(BattleTanksLibrary PUBLIC GodotCpp::GodotCpp ClipperLib PolyPartition)
target_compile_features(BattleTanksLibrary PUBLIC cxx_std_14)

# Configure Emscripten build
if(CMAKE_SYSTEM_NAME STREQUAL Emscripten)
	set_property(TARGET BattleTanksLibrary PROPERTY SUFFIX ".wasm")

	target_link_options(BattleTanksLibrary PRIVATE
		"SHELL:-s SIDE_MODULE=2"
		"SHELL:-s BINARYEN_TRAP_MODE='clamp'"
		"SHELL:-s EXPORTED_FUNCTIONS=['_godot_gdnative_init','_godot_gdnative_terminate','_godot_nativescript_init']"
	)
endif()

# Setup output name suffix for the Battle Tanks library target
if(CMAKE_SYSTEM_NAME STREQUAL Android)
	set(BATTLETANKS_SUFFIX "android.${CMAKE_ANDROID_ARCH_ABI}")
elseif(CMAKE_SYSTEM_NAME STREQUAL Emscripten)
	set(BATTLETANKS_SUFFIX "javascript")
else()
	if(WIN32)
		set(BATTLETANKS_SUFFIX "windows")
	elseif(UNIX OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
		set(BATTLETANKS_SUFFIX "x11")
	endif()

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(BATTLETANKS_SUFFIX "${BATTLETANKS_SUFFIX}.64")
	else()
		set(BATTLETANKS_SUFFIX "${BATTLETANKS_SUFFIX}.32")
	endif()
endif()

set_property(TARGET BattleTanksLibrary
	PROPERTY OUTPUT_NAME "battletanks.${BATTLETANKS_SUFFIX}"
)

# Add _DEBUG macro on Debug target build
# Some compilers doesn't provide this macro by default
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(BattleTanksLibrary PRIVATE _DEBUG)
endif()
