include(AwsTestHarness)
enable_testing()
include(CTest)

file(GLOB AWS_ECHOTESTRPC_HEADERS
        "include/awstest/*.h"
)

file(GLOB AWS_ECHOTESTRPC_SRC
        "EchoTestRpcClient.cpp"
        "EchoTestRpcModel.cpp"
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\awstest\\" FILES ${AWS_ECHOTESTRPC_HEADERS})
        source_group("Source Files" FILES ${AWS_ECHOTESTRPC_SRC})
    endif ()
endif()

if (UNIX AND NOT APPLE)
    include(GNUInstallDirs)
elseif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")

    if (${CMAKE_INSTALL_LIBDIR} STREQUAL "lib64")
        set(FIND_LIBRARY_USE_LIB64_PATHS true)
    endif()
endif()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_PREFIX_PATH}/${CMAKE_INSTALL_LIBDIR}/cmake")
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

install(FILES ${AWS_ECHOTESTRPC_HEADERS} DESTINATION "include/awstest/" COMPONENT Development)

file(GLOB TEST_SRC "*.cpp")
file(GLOB TEST_HDRS "*.h")
file(GLOB TESTS ${TEST_HDRS} ${TEST_SRC})

set(TEST_BINARY_NAME ${PROJECT_NAME}-tests)

aws_use_package(aws-crt-cpp)
aws_use_package(EventstreamRpc-cpp)
add_test_case(OperateWhileDisconnected)
# The tests below can be commented out when an EchoRPC Server is running on 127.0.0.1:8033
#add_test_case(EventStreamConnect)
#add_test_case(EchoOperation)
#add_test_case(StressTestClient)
generate_cpp_test_driver(${TEST_BINARY_NAME})
target_include_directories(${TEST_BINARY_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
