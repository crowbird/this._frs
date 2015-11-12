cmake_minimum_required(VERSION 2.8.11)
project(frs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/")

find_package(OpenCV REQUIRED)
find_package(OpenBR REQUIRED)
find_package(Tcmalloc)
find_package(Threads)

link_directories(/usr/lib/)

if (MSVC)
set(Boost_USE_STATIC_LIBS "On")
find_package( Boost 1.52 COMPONENTS date_time filesystem system thread regex REQUIRED )
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y")
find_package( Boost 1.52 COMPONENTS date_time filesystem system thread REQUIRED )
endif()

include_directories( ${Boost_INCLUDE_DIR} )
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y")

find_package(Qt5 5.4.0 REQUIRED Core Gui Xml )

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../include)

include_directories("${PROJECT_INCLUDE_DIR}")

SET(requiredlibs)

FIND_PACKAGE(CURL)
IF(CURL_FOUND)
  INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
  SET(requiredlibs ${requiredlibs} ${CURL_LIBRARIES} )
ELSE(CURL_FOUND)
  MESSAGE(FATAL_ERROR "Could not find the CURL library and development files.")
ENDIF(CURL_FOUND)

include_directories(${Qt5Widgets_INCLUDE_DIRS} 
                    ${Qt5Core_INCLUDE_DIRS} 
                    ${Qt5Gui_INCLUDE_DIRS})

add_executable(frs face.cpp node.cpp photo.cpp facebase.cpp eye.cpp server.cpp main.cpp)

target_link_libraries(frs ${requiredlibs} ${Boost_LIBRARIES} ${OpenCV_LIBS} ${OPENBR_LIBS} Qt5::Core) 
