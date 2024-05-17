file(GLOB 
        HEADER_LIST 
        CONFIGURE_DEPENDS 
        "${DSBAProj_SOURCE_DIR}/include/invest/*.hpp" 
        "${DSBAProj_SOURCE_DIR}/include/invest/*.h")

add_library(invest_lib invest/example.cpp ${HEADER_LIST})
target_include_directories(invest_lib PUBLIC ../include)
target_link_libraries(invest_lib PRIVATE tink_grpc_proto TinkoffInvestSDK)



# target_link_libraries(ort ortools::ortools)








