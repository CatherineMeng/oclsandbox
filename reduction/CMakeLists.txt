copyKernel(reduce_sum.cl)
add_executable(reducer reducer.cpp)
target_link_libraries(reducer OpenCL::OpenCL ${boardlibs})
compileKernel(reducer reduce_sum ON)
if(KERNEL_CIRCUIT STREQUAL "ON")
  compileKernel(reducer reduce_sum OFF)
endif()
