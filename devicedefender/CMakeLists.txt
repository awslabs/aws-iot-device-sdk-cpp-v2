cmake_minimum_required(VERSION 3.1)
project(IotDeviceDefender-cpp CXX C)

set(RUNTIME_DIRECTORY bin)

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

file(GLOB AWS_IOTDEVICEDEFENDER_HEADERS
        "include/aws/iotdevicedefender/*.h"
        )

file(GLOB AWS_IOTDEVICEDEFENDER_SRC
        "source/*.cpp"
        )

file(GLOB AWS_IOTDEVICEDEFENDER_CPP_SRC
        ${AWS_IOTDEVICEDEFENDER_SRC}
        )

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\iotdevicedefender\\" FILES ${AWS_IOTDEVICEDEFENDER_HEADERS})

        source_group("Source Files" FILES ${AWS_IOTDEVICEDEFENDER_SRC})
    endif ()
endif()

add_library(IotDeviceDefender-cpp ${AWS_IOTDEVICEDEFENDER_CPP_SRC})

set_target_properties(IotDeviceDefender-cpp PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(IotDeviceDefender-cpp PRIVATE /W4 /WX)
else ()
    target_compile_options(IotDeviceDefender-cpp PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(IotDeviceDefender-cpp PRIVATE "-DDEBUG_BUILD")
endif ()

if (BUILD_SHARED_LIBS)
    target_compile_definitions(IotDeviceDefender-cpp PUBLIC "-DAWS_IOTDEVICEDEFENDER_USE_IMPORT_EXPORT")
    target_compile_definitions(IotDeviceDefender-cpp PRIVATE "-DAWS_IOTDEVICEDEFENDER_EXPORTS")

    install(TARGETS IotDeviceDefender-cpp
            EXPORT IotDeviceDefender-cpp-targets
            ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_SKIP
            COMPONENT Runtime
            RUNTIME
            DESTINATION ${RUNTIME_DIRECTORY}
            COMPONENT Runtime)

    install(TARGETS IotDeviceDefender-cpp
            EXPORT IotDeviceDefender-cpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS IotDeviceDefender-cpp
            EXPORT IotDeviceDefender-cpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(IotDeviceDefender-cpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (BUILD_DEPS)
	if (NOT IS_SUBDIRECTORY_INCLUDE)
		aws_use_package(aws-crt-cpp)
        endif()
	aws_use_package(aws-c-iot)
endif()

if (BUILD_TESTING)
    aws_use_package(IotDeviceCommon-cpp)
endif()

target_link_libraries(IotDeviceDefender-cpp ${DEP_AWS_LIBS})

install(FILES ${AWS_IOTDEVICEDEFENDER_HEADERS} DESTINATION "include/aws/iotdevicedefender/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set(TARGET_DIR "shared")
else()
    set(TARGET_DIR "static")
endif()

install(EXPORT "IotDeviceDefender-cpp-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/IotDeviceDefender-cpp/cmake/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/iotdevicedefender-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/iotdevicedefender-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/iotdevicedefender-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/IotDeviceDefender-cpp/cmake/"
        COMPONENT Development)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
