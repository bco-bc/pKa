
cmake_minimum_required(VERSION 2.8)

project ( cgal-triangulator )

set( Boost_USE_MULTITHREADED ON )

find_package( CGAL QUIET COMPONENTS )

if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  

endif()

include( ${CGAL_USE_FILE} )

set ( Eigen3_DIR /localdisk/local/share/eigen3/cmake )
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

set (CMAKE_CXX_FLAGS "-Wall -O2 -pg -std=c++14 -lfftw3")

include_directories(include/)
include_directories(/home/pleprovo/code/sim-util/include/)

set( SOURCE_TESTS
  src/cgal-triangulator.cpp
  src/tetrahedron-triangulator.cpp
  src/vertex.cpp
  src/edge.cpp
  src/triangle.cpp
  src/surface.cpp
  src/triangulated-surface.cpp)

add_executable ( test-cgal-triangulator tests/test-cgal-triangulator.cpp ${SOURCE_TESTS} )
add_executable ( test-surface tests/test-surface.cpp ${SOURCE_TESTS})

target_link_libraries( test-cgal-triangulator Eigen3::Eigen ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} )
target_link_libraries( test-surface Eigen3::Eigen ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} )

