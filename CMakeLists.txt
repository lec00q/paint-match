CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(paintingMatcher)
SET(paintingMatcher_VERSION_MAJOR 0)
SET(paintingMatcher_VERSION_MINOR 1)

# Guard against in-source builds
IF (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	MESSAGE (FATAL_ERROR "In-source builds not allowed.
		 Please make a new directory (called a build directory)
		 and run CMake from there. You may need to remove
		 CMakeCache.txt.")
ENDIF ()

# SET COMPILER OPTIONS
IF(CMAKE_COMPILER_IS_GNUCXX)

    # General compile definitions
    SET (CMAKE_C_FLAGS_DEBUG "-pg -g -Wall")
    SET (CMAKE_EXE_LINKER_FLAGS_DEBUG "-pg")
    SET (CMAKE_C_FLAGS_RELEASE "-g0 -DNDEBUG -O3")

    # Copy C flags to C++ compiler flags
    SET (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
    SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# Make sure the compiler can find include files.
CMAKE_POLICY (SET CMP0015 NEW)

# Build shared lib and set RPATH (Linux only)
#SET(BUILD_SHARED_LIBS ON)
#SET(CMAKE_INSTALL_RPATH "${PROJECT_SOURCE_DIR}/lib")

# Configure a header file to pass some of the CMake settings
# to the source code
CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/config.h.in"
               "${PROJECT_BINARY_DIR}/config.h")

# Add the binary tree to the search path for include files
# so that we will find config.h
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

# INCLUDE DIRECTORIES
# When  including  directories  in  the root  CMakeList,  the  path  are
# automatically exported  to all  subdirectories, so we  do not  need to
# include the paths again!

# Include directories (-I)
INCLUDE_DIRECTORIES (src)
# Make sure the linker can find the libraries once built (-L)
LINK_DIRECTORIES (src)

ADD_SUBDIRECTORY (src)

SET(CPACK_GENERATOR "TBZ2")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Piero Donaggio") #required
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "paintMatcher")
SET(CPACK_PACKAGE_VENDOR "Trya Srl")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)

INCLUDE (CPack)
