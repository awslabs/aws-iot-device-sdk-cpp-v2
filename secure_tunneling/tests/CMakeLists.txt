include(AwsTestHarness)
enable_testing()
include(CTest)

file(GLOB TEST_SRC "*.cpp")
file(GLOB TEST_HDRS "*.h")
file(GLOB TESTS ${TEST_HDRS} ${TEST_SRC})

set(TEST_BINARY_NAME ${PROJECT_NAME}-tests)

aws_use_package(aws-crt-cpp)

if (UNIX AND NOT APPLE)
    add_test_case(SecureTunnelingHandleStreamStartTest)
    add_test_case(SecureTunnelingHandleDataReceiveTest)
    add_test_case(SecureTunnelingHandleStreamResetTest)
    add_test_case(SecureTunnelingHandleSessionResetTest)
    generate_cpp_test_driver(${TEST_BINARY_NAME})
endif()
