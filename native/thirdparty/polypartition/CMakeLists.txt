# PolyPartition library provides triangulation and convex hull alghorithms
# Library implements Ear Clipping and Hertel-Mehlhorn alghorithms
# Emitted targets: PolyPartition

add_library(PolyPartition STATIC
	polypartition.cpp
)

target_include_directories(PolyPartition PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_features(PolyPartition PUBLIC
	cxx_std_14
)

set_target_properties(PolyPartition PROPERTIES
	OUTPUT_NAME "polypartition"
)
