find_package(tftp_common)
find_package(GTest)

add_executable(packets_test packets_test.cpp)

target_link_libraries(packets_test
	PRIVATE GTest::GTest tftp_common::tftp_common
)

add_test(packets_gtests packets_test)