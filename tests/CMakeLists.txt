find_package(GTest)

add_executable(packets_test packets_test.cpp)

target_link_libraries(packets_test
	PRIVATE GTest::GTest
)

add_test(packets_gtests packets_test)