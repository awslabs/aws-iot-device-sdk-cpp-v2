cmake_minimum_required(VERSION 3.1)
project(EventstreamRpc-cpp CXX)

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

file(GLOB AWS_EVENTSTREAMRPC_HEADERS
        "include/aws/eventstreamrpc/*.h"
)

file(GLOB AWS_EVENTSTREAMRPC_SRC
       "source/*.cpp"
)

file(GLOB AWS_EVENTSTREAMRPC_CPP_SRC
        ${AWS_EVENTSTREAMRPC_SRC}
)

if (WIN32)
    if (MSVC)
        source_group("Header Files\\aws\\eventstreamrpc\\" FILES ${AWS_EVENTSTREAMRPC_HEADERS})

        source_group("Source Files" FILES ${AWS_EVENTSTREAMRPC_SRC})
    endif ()
endif()

add_library(EventstreamRpc-cpp ${AWS_EVENTSTREAMRPC_CPP_SRC})

set_target_properties(EventstreamRpc-cpp PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_C_FLAGS_DEBUGOPT "")

#set warnings
if (MSVC)
    target_compile_options(EventstreamRpc-cpp PRIVATE /W4 /WX)
else ()
    target_compile_options(EventstreamRpc-cpp PRIVATE -Wall -Wno-long-long -pedantic -Werror)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(EventstreamRpc-cpp PRIVATE "-DDEBUG_BUILD")
endif ()

if (BUILD_SHARED_LIBS)
    target_compile_definitions(EventstreamRpc-cpp PUBLIC "-DAWS_EVENTSTREAMRPC_USE_IMPORT_EXPORT")
    target_compile_definitions(EventstreamRpc-cpp PRIVATE "-DAWS_EVENTSTREAMRPC_EXPORTS")

    install(TARGETS EventstreamRpc-cpp
            EXPORT EventstreamRpc-cpp-targets
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

    install(TARGETS EventstreamRpc-cpp
            EXPORT EventstreamRpc-cpp-targets
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            NAMELINK_ONLY
            COMPONENT Development)
else()
    install(TARGETS EventstreamRpc-cpp
            EXPORT EventstreamRpc-cpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT Development)
endif()

target_include_directories(EventstreamRpc-cpp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

if (NOT IS_SUBDIRECTORY_INCLUDE)
    aws_use_package(aws-crt-cpp)
endif()


target_link_libraries(EventstreamRpc-cpp ${DEP_AWS_LIBS})

install(FILES ${AWS_EVENTSTREAMRPC_HEADERS} DESTINATION "include/aws/eventstreamrpc/" COMPONENT Development)

if (BUILD_SHARED_LIBS)
    set(TARGET_DIR "shared")
else()
    set(TARGET_DIR "static")
endif()

install(EXPORT "EventstreamRpc-cpp-targets"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/EventstreamRpc-cpp/cmake/${TARGET_DIR}"
        NAMESPACE AWS::
        COMPONENT Development)

configure_file("cmake/eventstreamrpc-cpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/eventstreamrpc-cpp-config.cmake"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/eventstreamrpc-cpp-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/EventstreamRpc-cpp/cmake/"
        COMPONENT Development)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
