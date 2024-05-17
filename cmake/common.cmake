find_package(Threads REQUIRED)

include(FetchContent)


FetchContent_Declare(TinkoffInvestSDK GIT_REPOSITORY https://github.com/samoilovv/TinkoffInvestSDK GIT_TAG main) 
FetchContent_MakeAvailable(TinkoffInvestSDK)
FetchContent_GetProperties(TinkoffInvestSDK)
# ortools
# FetchContent_Declare(ortools GIT_REPOSITORY https://github.com/google/or-tools.git GIT_TAG stable) 
# FetchContent_MakeAvailable(ortools)

# ta-lib
