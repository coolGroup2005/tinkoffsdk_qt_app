cmake_minimum_required(VERSION 3.8)

project(tinkoff-app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Charts Sql Concurrent)
find_package(SQLite3 REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    portfolio.cpp
    portfolio.h
    figi.h 
    figi.cpp
    favouritesModel.cpp
    favouritesModel.h
    favouritesProxyModel.cpp
    favouritesProxyModel.h
)

add_executable(tinkoff-app ${PROJECT_SOURCES} mainwindow.ui portfolio.ui figi.h figi.cpp figi.ui)

target_link_libraries(tinkoff-app PRIVATE
    Qt6::Widgets
    Qt6::PrintSupport
    Qt6::Charts
    Qt6::Sql
    Qt6::Concurrent
    homepage_lib
    welcome_lib
    shares_lib
    statistics_lib
    tink_grpc_proto 
    TinkoffInvestSDK 
    sqlite3
)

set_target_properties(tinkoff-app PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)

if (LINUX)
    install(TARGETS tinkoff-app RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
