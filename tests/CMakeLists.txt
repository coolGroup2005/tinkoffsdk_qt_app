# FetchContent_Declare(
#   googletest
#   GIT_REPOSITORY https://github.com/google/googletest.git
#   GIT_TAG f8d7d77c06936315286eb55f8de22cd23c188571
# )
# FetchContent_MakeAvailable(googletest)

# add_executable(testlib test.cpp)
# target_link_libraries(testlib PRIVATE proj_lib GTest::gtest_main)

# include(GoogleTest)
# gtest_discover_tests(testlib)



# add_executable(testlibC ctest.cpp)
# target_link_libraries(testlibC PRIVATE proj_lib)
# enable_testing()
# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

# add_test(
#   NAME factorial
#   COMMAND $<TARGET_FILE:testlibC>
#   )