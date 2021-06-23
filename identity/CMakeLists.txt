cmake_minimum_required(VERSION 3.1)
project(IotIdentity-cpp CXX)

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

file(GLOB AWS_IOTIDENTITY_HEADERS
        "include/aws/iotidentity/*.h"
)

file(GLOB AWS_IOTIDENTITY_SRC
       "source/*.cpp"
)

file(GLOB AWS_IOTIDENTITY_CPP_SRC
        ${AWS_IOTIDENTITY_SRC}
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\iotidentity\\" FILES ${AWS_IOTIDENTITY_HEADERS})

        source_group("Source Files" FILES ${AWS_IOTIDENTITY_SRC})
    endif ()
endif()

add_library(IotIdentity-cpp ${AWS_IOTIDENTITY_CPP_SRC})

set_target_properties(IotIdentity-cpp PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(IotIdentity-cpp PRIVATE /W4 /WX)
else ()
    target_compile_options(IotIdentity-cpp PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(IotIdentity-cpp PRIVATE "-DDEBUG_BUILD")
endif ()

if (BUILD_SHARED_LIBS)
    target_compile_definitions(IotIdentity-cpp PUBLIC "-DAWS_IOTIDENTITY_USE_IMPORT_EXPORT")
    target_compile_definitions(IotIdentity-cpp PRIVATE "-DAWS_IOTIDENTITY_EXPORTS")

    install(TARGETS IotIdentity-cpp
            EXPORT IotIdentity-cpp-targets
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

    install(TARGETS IotIdentity-cpp
            EXPORT IotIdentity-cpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS IotIdentity-cpp
            EXPORT IotIdentity-cpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(IotIdentity-cpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (BUILD_DEPS)
	if (NOT IS_SUBDIRECTORY_INCLUDE)
		aws_use_package(aws-crt-cpp)
	endif()
endif()

target_link_libraries(IotIdentity-cpp ${DEP_AWS_LIBS})

install(FILES ${AWS_IOTIDENTITY_HEADERS} DESTINATION "include/aws/iotidentity/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set(TARGET_DIR "shared")
else()
    set(TARGET_DIR "static")
endif()

install(EXPORT "IotIdentity-cpp-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/IotIdentity-cpp/cmake/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/iotidentity-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/iotidentity-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/iotidentity-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/IotIdentity-cpp/cmake/"
        COMPONENT Development)
