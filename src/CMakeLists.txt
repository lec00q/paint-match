######################################
# FIND EXTERNAL LIBRARIES
# Here we ask CMake to find all the external libraries we need. Doing so
# in  the root  CMakeList allows  us  to use  the libraries  in all  the
# subdirectories
######################################

# OPENCV
FIND_PACKAGE (OpenCV REQUIRED)
INCLUDE_DIRECTORIES (${OpenCV_INCLUDE_DIRS})

# BOOST
SET (Boost_USE_STATIC_LIBS ON)
UNSET (Boost_INCLUDE_DIR CACHE)
UNSET (Boost_LIBRARY_DIRS CACHE)
FIND_PACKAGE (Boost COMPONENTS system filesystem REQUIRED)
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})

# GOOGLETEST
#FIND_PACKAGE (GTest REQUIRED)
#IF (GTEST_FOUND)
#    INCLUDE_DIRECTORIES (${GTEST_INCLUDE_DIR})
#    ENABLE_TESTING()
#ENDIF (GTEST_FOUND)

FIND_PACKAGE (ZLIB REQUIRED)
INCLUDE_DIRECTORIES (${ZLIB_INCLUDE_DIRS})

FIND_PACKAGE (TIFF)
INCLUDE_DIRECTORIES (${TIFF_INCLUDE_DIRS})

FIND_PACKAGE (PNG)
INCLUDE_DIRECTORIES (${PNG_INCLUDE_DIRS})

FIND_PACKAGE (JPEG)
INCLUDE_DIRECTORIES (${JPEG_INCLUDE_DIRS})

FIND_PACKAGE (Threads REQUIRED)

######################################
# CREATE LIBRARIES AND EXECUTABLES
######################################

SET (SRCS ImageReader.cpp ImageMatcher.cpp)

ADD_EXECUTABLE (paintMatcher main.cpp ${SRCS})
TARGET_LINK_LIBRARIES (paintMatcher
/usr/local/lib/libopencv_nonfree.a
/usr/local/lib/libopencv_calib3d.a
/usr/local/lib/libopencv_flann.a
/usr/local/lib/libopencv_highgui.a
/usr/local/lib/libopencv_features2d.a
/usr/local/lib/libopencv_imgproc.a
/usr/local/lib/libopencv_core.a
/usr/local/share/OpenCV/3rdparty/lib/libIlmImf.a
/usr/local/share/OpenCV/3rdparty/lib/liblibjasper.a
${TIFF_LIBRARIES} ${JPEG_LIBRARIES}
${ZLIB_LIBRARIES} ${PNG_LIBRARIES}
${Boost_LIBRARIES} rt)

#ADD_EXECUTABLE (paintMatcherTest ImageMatcherTest.cc ${SRCS})
#TARGET_LINK_LIBRARIES (paintMatcherTest ${OpenCV_LIBS} ${Boost_LIBRARIES} ${GTEST_BOTH_LIBRARIES})

INSTALL (TARGETS paintMatcher RUNTIME DESTINATION .)

