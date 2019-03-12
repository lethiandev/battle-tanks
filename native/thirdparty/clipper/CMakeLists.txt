# ClipperLib is a library to perform polygons clipping
# The library is based on Vatti's clipping algorithm
# Emitted targets: ClipperLib

add_library(ClipperLib STATIC
	clipper.cpp
)

target_include_directories(ClipperLib PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_features(ClipperLib PUBLIC
	cxx_std_14
)

set_target_properties(ClipperLib PROPERTIES
	OUTPUT_NAME "clipper"
)
