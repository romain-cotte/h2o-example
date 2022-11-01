cmake_minimum_required(VERSION 3.16)
project(H2O_example)

set(USE_SANTINIZER ON)

set(CMAKE_BUILD_TYPE Debug)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -Wno-unused-parameter -Werror=return-type -O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wno-unused-parameter -Werror=return-type -std=c++17 -O0 -g -fsanitize=address")


message(STATUS "compiler_id: ${CMAKE_CXX_COMPILER_ID}")
if(USE_SANTINIZER)
  message(STATUS "set CMAKE_CXX_FLAGS_DEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize-recover=all -fPIE")
endif()

set(DEP_ROOT_DIR ${CMAKE_SOURCE_DIR}/external-${CMAKE_SYSTEM_NAME})


include(cmake/H2O.cmake)

FIND_PACKAGE(OpenSSL 1.1.1 REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)

message("OpenSSL library: ${OPENSSL_LIBRARIES}")


message(STATUS "include directories ${DEP_ROOT_DIR}/${H2O_NAME}/include")
include_directories(${DEP_ROOT_DIR}/${H2O_NAME}/include)
include_directories(${DEP_ROOT_DIR}/${H2O_NAME}/include/h2o)

link_directories(${DEP_ROOT_DIR}/${H2O_NAME}/build)

add_executable(main src/main.cpp)


target_include_directories(main PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

set(STD_LIB -static-libgcc -static-libstdc++)

set(CORE_LIBS h2o-evloop ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES} pthread ${STD_LIB})

target_link_libraries(main ${CORE_LIBS})
