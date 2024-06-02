cmake_minimum_required(VERSION 3.8)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Charts Sql)
find_package(SQLite3 REQUIRED)

set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

set(HOMEPAGE_SOURCES
    homepage/homepage.cpp
    ${PROJECT_INCLUDE_DIR}/homepage/homepage.h
)

set(SHARES_SOURCES
    shares/shares.cpp
    ${PROJECT_INCLUDE_DIR}/shares/shares.h
)

set(WELCOME_SOURCES
    welcome/welcome.cpp
    ${PROJECT_INCLUDE_DIR}/welcome/welcome.h
)

set(STATISTICS_SOURCES
    statistics/statistics.cpp
    ${PROJECT_INCLUDE_DIR}/statistics/statistics.h
)

# Define library targets
add_library(homepage_lib ${HOMEPAGE_SOURCES})
add_library(shares_lib ${SHARES_SOURCES})
add_library(welcome_lib ${WELCOME_SOURCES})
add_library(statistics_lib ${WELCOME_SOURCES})


# Include directories for all library targets
target_include_directories(homepage_lib PUBLIC ${PROJECT_INCLUDE_DIR})
target_include_directories(shares_lib PUBLIC ${PROJECT_INCLUDE_DIR})
target_include_directories(welcome_lib PUBLIC ${PROJECT_INCLUDE_DIR})
target_include_directories(statistics_lib PUBLIC ${PROJECT_INCLUDE_DIR})

# Link libraries
target_link_libraries(homepage_lib PRIVATE tink_grpc_proto TinkoffInvestSDK Qt6::Widgets)
target_link_libraries(shares_lib PRIVATE tink_grpc_proto TinkoffInvestSDK Qt6::Widgets Qt6::Charts Qt6::PrintSupport Qt6::Sql sqlite3)
target_link_libraries(welcome_lib PRIVATE Qt6::Widgets Qt6::PrintSupport)
target_link_libraries(statistics_lib PRIVATE  tink_grpc_proto TinkoffInvestSDK Qt6::Widgets Qt6::Sql sqlite3)
