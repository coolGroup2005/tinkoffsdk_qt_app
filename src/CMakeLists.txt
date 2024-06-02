cmake_minimum_required(VERSION 3.8)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts PrintSupport Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport Charts Sql)
find_package(SQLite3 REQUIRED)

set(PROJECT_SOURCES ${CMAKE_SOURCE_DIR})

file(GLOB 
        HEADER_LIST 
        CONFIGURE_DEPENDS 
        "${DSBAProj_SOURCE_DIR}/include/invest/*.hpp" 
        "${DSBAProj_SOURCE_DIR}/include/invest/*.h"
        )

add_library(invest_lib 
        invest/example.cpp 
        ${HEADER_LIST}
        )

target_include_directories(invest_lib 
        PUBLIC 
        ../include
        ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS}
        )

target_link_libraries(invest_lib 
        PRIVATE 
        tink_grpc_proto 
        TinkoffInvestSDK
        Qt${QT_VERSION_MAJOR}::Widgets
        )

file(GLOB 
        HEADER_LIST2 
        CONFIGURE_DEPENDS
        "${DSBAProj_SOURCE_DIR}/include/homepage/*.h" 
)

add_library(homepage_lib 
        homepage/homepage.cpp 
        ${HEADER_LIST2} 
        )

file(GLOB HEADER_LIST3
        CONFIGURE_DEPENDS
        "${DSBAProj_SOURCE_DIR}/include/shares/*.h"
    )

add_library(shares_lib 
        shares/shares.cpp
        ../include/shares/shares.h
        ${HEADER_LIST3}
        )

target_include_directories(homepage_lib 
        PUBLIC 
        ../include
        ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS}
        )

target_link_libraries(homepage_lib 
        PRIVATE 
        tink_grpc_proto 
        TinkoffInvestSDK
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        sqlite3
        )

target_include_directories(shares_lib 
        PUBLIC 
        ../include
        ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS}
        )

target_link_libraries(shares_lib 
        PRIVATE 
        tink_grpc_proto 
        TinkoffInvestSDK
        Qt${QT_VERSION_MAJOR}::Widgets 
        Qt${QT_VERSION_MAJOR}::PrintSupport
        Qt${QT_VERSION_MAJOR}::Charts
        Qt${QT_VERSION_MAJOR}::Sql
        sqlite3
        )

target_sources(shares_lib
    PRIVATE
    ${UI_HEADERS}
)

file(GLOB 
        HEADER_LIST4 
        CONFIGURE_DEPENDS
        "${DSBAProj_SOURCE_DIR}/include/statistics/*.h" 
)

add_library(statistics_lib 
        statistics/statistics.cpp 
        ${HEADER_LIST4} 
        )

target_include_directories(statistics_lib 
        PUBLIC 
        ../include
        ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS}
        )

target_link_libraries(statistics_lib 
        PRIVATE 
        tink_grpc_proto 
        TinkoffInvestSDK
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        sqlite3
        )
