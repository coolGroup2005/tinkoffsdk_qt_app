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
        ../include)

# link invest_lib along with other required libraries
target_link_libraries(invest_lib 
        PRIVATE # The PRIVATE keyword means that the library will be used by this project, but it will not be available to other projects via this project’s interface
        tink_grpc_proto 
        TinkoffInvestSDK
        )

# the file() command is used to find all header files for homepage_lib
file(GLOB 
        HEADER_LIST2 # the variable where the list of files will be stored
        CONFIGURE_DEPENDS
        # list of header files to add into list HEADER_LIST2, ${DSBAProj_SOURCE_DIR} = path to folder with this repository on your computer
        "${DSBAProj_SOURCE_DIR}/include/homepage/*.h" 
)

# define homepage_lib
add_library(homepage_lib #name of your library
        homepage/homepage.cpp #if >1, try adding them to a list first?
        ${HEADER_LIST2} #name of variable from respective file()
        )

# set include directories for homepage_lib
target_include_directories(homepage_lib 
        PUBLIC # Paths in PUBLIC section are used by projects that link to this library
        ../include
        )

target_link_libraries(homepage_lib 
        PRIVATE # The PRIVATE keyword means that the library will be used by this project, but it will not be available to other projects via this project’s interface
        tink_grpc_proto 
        TinkoffInvestSDK
        )

# target_link_libraries(ort ortools::ortools)
