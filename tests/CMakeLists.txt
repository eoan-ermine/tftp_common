find_package(GTest)

add_executable(packets_test packets_test.cpp)
add_executable(parse_test parse_test.cpp)

target_link_libraries(packets_test PRIVATE GTest::GTest)
target_link_libraries(parse_test PRIVATE GTest::GTest)

add_test(packets_gtests packets_test)
add_test(parse_gtests parse_test)