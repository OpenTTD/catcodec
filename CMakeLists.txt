cmake_minimum_required(VERSION 3.16)

project(catcodec)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_CXX_EXTENSIONS NO)

if(MINGW)
	# Force searching static libs, so the executables can run outside MinGW environment
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

	# Force static linking, so the executables can run outside MinGW environment
	link_libraries(-static -static-libgcc -static-libstdc++)
endif()

if(MSVC)
	# Switch to MT (static) instead of MD (dynamic) binary

	# For MSVC two generators are available
	# - a command line generator (Ninja) using CMAKE_BUILD_TYPE to specify the
	#   configuration of the build tree
	# - an IDE generator (Visual Studio) using CMAKE_CONFIGURATION_TYPES to
	#   specify all configurations that will be available in the generated solution
	list(APPEND MSVC_CONFIGS "${CMAKE_BUILD_TYPE}" "${CMAKE_CONFIGURATION_TYPES}")

	# Set usage of static runtime for all configurations
	foreach(MSVC_CONFIG ${MSVC_CONFIGS})
		string(TOUPPER "CMAKE_CXX_FLAGS_${MSVC_CONFIG}" MSVC_FLAGS)
		string(REPLACE "/MD" "/MT" ${MSVC_FLAGS} "${${MSVC_FLAGS}}")
	endforeach()

	add_compile_options(
		/wd4996 # Disable deprecation warnings
		/W3
	)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	add_compile_options(
		-Wall
		-Wextra
		-Wno-format-nonliteral
		-Wno-multichar
	)
endif()


# Add some -D flags for Debug builds. We cannot use add_definitions(), because
# it does not appear to support the $<> tags.
add_compile_options(
    "$<$<CONFIG:Debug>:-D_DEBUG>"
    "$<$<NOT:$<CONFIG:Debug>>:-D_FORTIFY_SOURCE=2>" # FORTIFY_SOURCE should only be used in non-debug builds (requires -O1+)
)
if(MINGW)
    add_link_options(
        "$<$<NOT:$<CONFIG:Debug>>:-fstack-protector>" # Prevent undefined references when _FORTIFY_SOURCE > 0
    )
endif()

# Prepare generated dir
set(GENERATED_BINARY_DIR "${CMAKE_BINARY_DIR}/generated")
include_directories("${GENERATED_BINARY_DIR}")

# Target to generate version.h
add_custom_target(version_header
	COMMAND ${CMAKE_COMMAND}
		-DGENERATED_BINARY_DIR=${GENERATED_BINARY_DIR}
		-DCPACK_BINARY_DIR=${CMAKE_BINARY_DIR}
		-P "${CMAKE_CURRENT_SOURCE_DIR}/generate_version.cmake"
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	BYPRODUCTS "${GENERATED_BINARY_DIR}/version.h"
)


# Create catcodec
add_executable(catcodec)
add_dependencies(catcodec version_header)

# Add source files
add_subdirectory(src)


# Install files
include(GNUInstallDirs)

install(TARGETS
	catcodec
	DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES
	${CMAKE_CURRENT_SOURCE_DIR}/changelog.txt
	${CMAKE_CURRENT_SOURCE_DIR}/COPYING
	DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

add_subdirectory(docs)


# CPack
if(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(ARCHITECTURE "win64")
	else()
		set(ARCHITECTURE "win32")
	endif()

	set(CPACK_SYSTEM_NAME "${ARCHITECTURE}")
	set(CPACK_STRIP_FILES YES)
	set(CPACK_OUTPUT_FILE_PREFIX "bundles")
	set(CPACK_PACKAGE_FILE_NAME "catcodec-#CPACK_PACKAGE_VERSION#-windows-${CPACK_SYSTEM_NAME}")

	set(CPACK_GENERATOR "ZIP")

	include(CPack)
endif()
