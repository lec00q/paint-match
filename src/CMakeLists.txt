######################################
# FIND EXTERNAL LIBRARIES
# Here we ask CMake to find all the external libraries we need. Doing so
# in  the root  CMakeList allows  us  to use  the libraries  in all  the
# subdirectories
######################################

# OPENCV
FIND_PACKAGE (OpenCV REQUIRED)
INCLUDE_DIRECTORIES (${OpenCV_INCLUDE_DIRS})

# GLOG (logging library)
FIND_PACKAGE(GLog REQUIRED)
IF (GLOG_FOUND)
    INCLUDE_DIRECTORIES (${GLOG_INCLUDE_DIR})
ENDIF (GLOG_FOUND)

# BOOST
SET (BOOST_LIBS system filesystem)
FIND_PACKAGE (Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})

######################################
# CREATE LIBRARIES AND EXECUTABLES
######################################

# Create the library for reading images
ADD_LIBRARY (image-reader ImageReader.cpp)
TARGET_LINK_LIBRARIES (image-reader ${OpenCV_LIBS} ${GLOG_LIBRARIES} ${Boost_LIBRARIES})

# Create main executable
ADD_EXECUTABLE (paintMatcher main.cpp)
TARGET_LINK_LIBRARIES (paintMatcher ${GLOG_LIBRARIES})

INSTALL (TARGETS image-reader paintMatcher
         RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin
         LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/lib
         ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/lib)
