cmake_minimum_required(VERSION 3.20)
project("test-clblast")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenCL REQUIRED)

if(OpenCL_FOUND)
    message(STATUS "OpenCL headers location: ${OpenCL_INCLUDE_DIRS}")
    message(STATUS "OpenCL libraries location: ${OpenCL_LIBRARIES}")
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/OpenCL-CLHPP)
        execute_process(
            COMMAND git clone https://github.com/KhronosGroup/OpenCL-CLHPP.git
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            RESULT_VARIABLE clone_result
        )
        if(clone_result EQUAL "0")
            message("KhronosGroup/OpenCL-CLHPP cloned successfully")
            include_directories(${CMAKE_BINARY_DIR}/OpenCL-CLHPP/include)
        else()
            message("Failed to clone KhronosGroup/OpenCL-CLHPP")
        endif()
    else()
        include_directories(${CMAKE_BINARY_DIR}/OpenCL-CLHPP/include)
    endif()

    add_executable(sgemm sgemm.cpp)
    add_executable(cgemm cgemm.cpp)
    set(LD_LIBRARIES "")
    if(APPLE)
        add_compile_definitions(HAVE_ACCELERATE)
        add_compile_definitions(ACCELERATE_NEW_LAPACK)
        list(APPEND LD_LIBRARIES "-framework OpenCL -framework Accelerate")
    else()
        list(APPEND LD_LIBRARIES "OpenCL")
    endif()

    find_package(OpenBLAS REQUIRED)
    if(OpenBLAS_FOUND)
        message(STATUS "OpenBLAS headers location: ${OpenBLAS_INCLUDE_DIRS}")
        message(STATUS "OpenBLAS libraries location: ${OpenBLAS_LIBRARIES}")
        include_directories(${OpenBLAS_INCLUDE_DIRS})
        list(APPEND LD_LIBRARIES ${OpenBLAS_LIBRARIES})
    endif()

    find_package(CLBlast REQUIRED)
    if (CLBlast_FOUND)
        list(APPEND LD_LIBRARIES "clblast")
    endif()

    target_link_libraries(sgemm ${LD_LIBRARIES})
    target_link_libraries(cgemm ${LD_LIBRARIES})
endif()
