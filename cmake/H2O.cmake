# Download and build H2O

message(STATUS "DEP_ROOT_DIR: ${DEP_ROOT_DIR}")

set(H2O_VERSION 3cf8bd5cb1e8ea8eb9030a358888113be690fd7f)

set(H2O_NAME h2o-${H2O_VERSION})
set(H2O_TAR_PATH ${DEP_ROOT_DIR}/${H2O_NAME}.tar.gz)

if(NOT EXISTS ${H2O_TAR_PATH})
    message(STATUS "Downloading https://github.com/h2o/h2o/archive/${H2O_VERSION}.tar.gz")
    file(DOWNLOAD https://github.com/h2o/h2o/archive/${H2O_VERSION}.tar.gz ${H2O_TAR_PATH})
endif()

if(NOT EXISTS ${DEP_ROOT_DIR}/${H2O_NAME})
    message(STATUS "Extracting ${H2O_NAME}...")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${H2O_TAR_PATH} WORKING_DIRECTORY ${DEP_ROOT_DIR}/)
endif()

message("Does the file ${DEP_ROOT_DIR}/${H2O_NAME}/build/libh2o-evloop.a exist?")

if(NOT EXISTS ${DEP_ROOT_DIR}/${H2O_NAME}/build/libh2o-evloop.a)
    message("Configuring ${H2O_NAME}...")
    file(MAKE_DIRECTORY ${DEP_ROOT_DIR}/${H2O_NAME}/build)

    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory deps/picotls/include include/h2o
                    WORKING_DIRECTORY ${DEP_ROOT_DIR}/${H2O_NAME})

    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory deps/quicly/include include/h2o
            WORKING_DIRECTORY ${DEP_ROOT_DIR}/${H2O_NAME})

    execute_process(COMMAND ${CMAKE_COMMAND}
            -DCMAKE_FIND_LIBRARY_SUFFIXES=.a
            -DCMAKE_CXX_FLAGS="-DNDEBUG"
            -DCMAKE_C_FLAGS="-DNDEBUG"
            -DWITH_MRUBY=off
            "-H${DEP_ROOT_DIR}/${H2O_NAME}"
            "-B${DEP_ROOT_DIR}/${H2O_NAME}/build"
            RESULT_VARIABLE
            H2O_CONFIGURE)

    message(STATUS "H2O_CONFIGURE ${H2O_CONFIGURE}")

    if(NOT H2O_CONFIGURE EQUAL 0)
        message(FATAL_ERROR "${H2O_NAME} configure failed!")
    endif()

    message(STATUS "BUILD_DEPS ${BUILD_DEPS}")

    if(BUILD_DEPS STREQUAL "yes")
        message("Building ${H2O_NAME} locally...")
        execute_process(COMMAND ${CMAKE_COMMAND} --build
                "${DEP_ROOT_DIR}/${H2O_NAME}/build"
                --target libh2o-evloop
                RESULT_VARIABLE
                H2O_BUILD)
        if(NOT H2O_BUILD EQUAL 0)
            message(FATAL_ERROR "${H2O_NAME} build failed!")
        endif()
    endif()
endif()
