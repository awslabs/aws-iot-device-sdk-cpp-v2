cmake_minimum_required(VERSION 3.1)
project(IotJobs-cpp CXX)

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

file(GLOB AWS_IOTJOBS_HEADERS
        "include/aws/iotjobs/*.h"
)

file(GLOB AWS_IOTJOBS_SRC
       "source/*.cpp"
)

file(GLOB AWS_IOTJOBS_CPP_SRC
        ${AWS_IOTJOBS_SRC}
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\iotjobs\\" FILES ${AWS_IOTJOBS_HEADERS})

        source_group("Source Files" FILES ${AWS_IOTJOBS_SRC})
    endif ()
endif()

add_library(IotJobs-cpp ${AWS_IOTJOBS_CPP_SRC})

set_target_properties(IotJobs-cpp PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(IotJobs-cpp PRIVATE /W4 /WX)
else ()
    target_compile_options(IotJobs-cpp PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(IotJobs-cpp PRIVATE "-DDEBUG_BUILD")
endif ()

if (BUILD_SHARED_LIBS)
    target_compile_definitions(IotJobs-cpp PUBLIC "-DAWS_IOTJOBS_USE_IMPORT_EXPORT")
    target_compile_definitions(IotJobs-cpp PRIVATE "-DAWS_IOTJOBS_EXPORTS")

    install(TARGETS IotJobs-cpp
            EXPORT IotJobs-cpp-targets
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

    install(TARGETS IotJobs-cpp
            EXPORT IotJobs-cpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS IotJobs-cpp
            EXPORT IotJobs-cpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(IotJobs-cpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (BUILD_DEPS)
	if (NOT IS_SUBDIRECTORY_INCLUDE)
		aws_use_package(aws-crt-cpp)
	endif()
endif()

target_link_libraries(IotJobs-cpp ${DEP_AWS_LIBS})

install(FILES ${AWS_IOTJOBS_HEADERS} DESTINATION "include/aws/iotjobs/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set(TARGET_DIR "shared")
else()
    set(TARGET_DIR "static")
endif()

install(EXPORT "IotJobs-cpp-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/IotJobs-cpp/cmake/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/iotjobs-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/iotjobs-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/iotjobs-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/IotJobs-cpp/cmake/"
        COMPONENT Development)
