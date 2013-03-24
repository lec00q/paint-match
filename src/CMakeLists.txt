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
SET (BOOST_LIBS system filesystem)
FIND_PACKAGE (Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})

# GOOGLETEST
FIND_PACKAGE (GTest REQUIRED)
IF (GTEST_FOUND)
    INCLUDE_DIRECTORIES (${GTEST_INCLUDE_DIR})
    ENABLE_TESTING()
ENDIF (GTEST_FOUND)

######################################
# CREATE LIBRARIES AND EXECUTABLES
######################################

SET (SRCS ImageReader.cpp ImageMatcher.cpp)

ADD_EXECUTABLE (paintMatcher main.cpp ${SRCS})
TARGET_LINK_LIBRARIES (paintMatcher ${OpenCV_LIBS} ${Boost_LIBRARIES})

ADD_EXECUTABLE (paintMatcherTest ImageMatcherTest.cc ${SRCS})
TARGET_LINK_LIBRARIES (paintMatcherTest ${OpenCV_LIBS} ${Boost_LIBRARIES} ${GTEST_BOTH_LIBRARIES})

INSTALL (TARGETS paintMatcher paintMatcherTest
         RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin
         LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/lib
         ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/lib)
