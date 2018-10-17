#Setting Version Number, Project Name
cmake_minimum_required (VERSION 2.8)
project(oclsandbox)

option(ENABLE_OPENCL_Altera "Build with Altera OpenCL support" OFF)
find_package(OpenCL REQUIRED)
find_path(OPENCLHPPDIR cl.hpp ${OPENCL_HPP_DIR})
if(OPENCLHPPDIR STREQUAL "OPENCLHPPDIR-NOTFOUND")
  message(FATAL_ERROR "OPENCLHPPDIR not found!")
endif()
message(STATUS "OPENCLHPPDIR ${OPENCLHPPDIR}")
set(boardlibs)
if (ENABLE_OPENCL_Altera)
  set(FPGA_BOARD CACHE STRING "name of FPGA board")
  option(KERNEL_CIRCUIT "Build kernel for execution on FPGA" OFF)
  option(KERNEL_PROFILE "Build with kernel profiling enabled" OFF)
  if(KERNEL_PROFILE STREQUAL "ON")
    set(profileArgs "--profile")
  endif()
  option(KERNEL_REPORT "Build with kernel reporting enabled" OFF)
  if(KERNEL_REPORT STREQUAL "ON")
    set(reportArgs "--report")
  endif()
  find_file(OCL_COMPILER aoc ${OPENCL_HPP_DIR})
  message(STATUS "opencl kernel compiler ${OCL_COMPILER}")
  find_library(nallaLib nalla_pcie_mmd)
  message(STATUS "libnalla_pcie_mmd ${nallaLib}")
  if(nallaLib STREQUAL "nallaLib-NOTFOUND")
    message(FATAL_ERROR "libnalla_pcie_mmd not found!")
  endif()
  find_library(alteraLib alteracl)
  if(alteraLib STREQUAL "alteraLib-NOTFOUND")
    message(FATAL_ERROR "libalteracl not found!")
  endif()
  find_library(alteraElfLib elf)
  if(alteraElfLib STREQUAL "alteraElfLib-NOTFOUND")
    message(FATAL_ERROR "Altera libelf not found!")
  endif()
  message(STATUS "Altera libelf ${alteraElfLib}")
  set(boardlibs ${nallaLib} ${alteraLib} ${alteraElfLib})
endif()

include_directories(${CMAKE_HOME_DIRECTORY}/opencl)
include_directories(${OPENCLHPPDIR})
message(STATUS "CMAKE_BINARY_DIR is ${CMAKE_BINARY_DIR}")
include_directories(${CMAKE_BINARY_DIR}/opencl) # opencl config header

macro(copyKernel kernel)
  file(COPY ${kernel} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/${kernelDest}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_SOURCE_DIR}/${kernel}
      ${CMAKE_BINARY_DIR}/${kernel}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${kernel})
endmacro()

macro(compileKernel host kernel emulate)
  set(emulatorArgs "-march=emulator")
  if(${emulate} STREQUAL "OFF")
    unset(emulatorArgs)
  endif()
  if (ENABLE_OPENCL_Altera)
    add_custom_command(
      TARGET ${host}
      POST_BUILD
      COMMAND ${OCL_COMPILER} --board ${FPGA_BOARD}
        ${emulatorArgs} ${profileArgs} ${reportArgs}
        ${CMAKE_CURRENT_BINARY_DIR}/${kernel}.cl
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  endif()
endmacro()

add_subdirectory(reduction)
