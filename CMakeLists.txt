cmake_minimum_required(VERSION 3.8)
project(DSBAProj VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(cmake/common.cmake)

# include(CTest)
# enable_testing()

add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(tests)

find_package(Doxygen)
if(Doxygen_FOUND)
    add_subdirectory(docs)
else()
    message(STATUS "Doxygen not found")
endif()






