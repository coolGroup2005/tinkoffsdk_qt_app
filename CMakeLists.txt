cmake_minimum_required(VERSION 3.8)
project(DSBAProj VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# Specific parameters for Linux CMAKE
if (LINUX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SYSTEMD REQUIRED libsystemd>=233)
endif()

include(cmake/common.cmake)

# include(CTest)
# enable_testing()

add_subdirectory(src)
add_subdirectory(apps)

find_package(Doxygen)
if(Doxygen_FOUND)
    add_subdirectory(docs)
else()
    message(STATUS "Doxygen not found")
endif()






