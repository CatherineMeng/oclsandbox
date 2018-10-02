#include <cassert>
#define __CL_ENABLE_EXCEPTIONS
#include <cl.hpp>
#include "util.hpp"
#include "device_picker.hpp"

namespace {

  cl::Context* engpar_ocl_context;
  cl::CommandQueue* engpar_ocl_queue;
  cl::Device* engpar_ocl_device;

  // copy buffer from the host to the device
  template <typename T>
  cl::Buffer* copyToDevice(T* in, int numVals, int mode) {
    cl::Buffer* d_buff = new cl::Buffer(*engpar_ocl_context, mode, numVals*sizeof(T));
    cl::copy(*engpar_ocl_queue, in, in+numVals, *d_buff);
    return d_buff;
  }

  // copy buffer from the device to the host
  template <typename T>
  void copyFromDevice(cl::Buffer* d_buff, T* h_buff, int numVals) {
    cl::copy(*engpar_ocl_queue, *d_buff, h_buff, h_buff+numVals);
  }

  void printBuildLogAndOpts(cl::Program* program) {
    std::string opts = program->getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(*engpar_ocl_device);
    std::string log = program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(*engpar_ocl_device);
    std::cerr << "----OpenCL Build Options----\n" << opts << std::endl;
    std::cerr << "----OpenCL Build Log--------\n" << log << std::endl;
    std::cerr << "----------------------------" << std::endl;
  }

  cl::Program* createProgram(std::string kernelFileName, int unroll=0) {
    std::string ext = kernelFileName.substr(kernelFileName.find_last_of(".") + 1);
    fprintf(stderr, "extension %s\n", ext.c_str());
    cl::Program* program;
    cl_int err = 0;
    if( ext == "cl" ) {
      program =
        new cl::Program(*engpar_ocl_context, util::loadProgram(kernelFileName));
      try {
        if( unroll )
          program->build("-DCSR_UNROLL -DSCG_UNROLL -Werror");
        else
          program->build("-Werror");
      } catch (cl::Error error) {
        std::cerr << "OpenCL Build Error..." << std::endl;
        err = 1;
      }
      printBuildLogAndOpts(program);
    } else if ( ext == "aocx" ) {
      std::vector<cl::Device> devices;
      devices.push_back(*engpar_ocl_device);
      std::string bytes = util::loadProgram(kernelFileName);
      cl::Program::Binaries bin;
      typedef std::pair<const void*, ::size_t> binPair;
      bin.push_back( binPair(bytes.c_str(),bytes.length()) );
      std::vector<cl_int> binStatus;
      program =
        new cl::Program(*engpar_ocl_context, devices, bin, &binStatus, &err);

      assert(binStatus.size() == 1);
      assert(binStatus[0] == CL_SUCCESS);
    } else {
      fprintf(stderr, "uknown kernel file extension... exiting\n");
      err = 1;
    }
    if ( err ) exit(EXIT_FAILURE);
    return program;
  }
}

int reduce_sum_gold(int* in, int sz) {
  int res = 0;
  for(int i=0; i<sz; i++)
    res += in[i];
  return res;
}

int main(int argc, char** argv) {

  std::cerr << "Inputs: ";
  for (int i = 1; i < argc; i++)
    std::cerr << argv[i] << " ";
  std::cerr << "\n";

  std::string kernelFileName;
  cl_uint deviceIndex = 0;
  try {
    parseArguments(argc,argv,&deviceIndex,kernelFileName);

    // Get list of devices
    std::vector<cl::Device> devices;
    getDeviceList(devices);

    // Check device index in range
    if (deviceIndex >= devices.size()) {
      std::cout << "Invalid device index (try '--list')" << std::endl;
      return 1;
    }

    engpar_ocl_device = new cl::Device(devices[deviceIndex]);

    std::string name = getDeviceName(*engpar_ocl_device);
    std::cout << std::endl << "Using OpenCL device: " << name << std::endl;

    engpar_ocl_context = new cl::Context(*engpar_ocl_device);
    engpar_ocl_queue = new cl::CommandQueue(*engpar_ocl_context);

  }
  catch (cl::Error err)
  {
    std::cout << "Exception:" << std::endl
              << "ERROR: "
              << err.what()
              << "("
              << err_code(err.err())
              << ")"
              << std::endl;
  }


  const int NUM_VALS = 1024;
  int* h_in = new int[NUM_VALS];
  for( int i=0; i< NUM_VALS; i++)
    h_in[i] = i;

  int unroll = 0;
  cl::Program* program = createProgram(kernelFileName,unroll);

  cl::make_kernel
    <cl::Buffer,       //input array
    cl_int,            //size of input array
    cl::Buffer>        //result
    reduce_sum(*program, "reduce_sum");

  cl::Buffer* d_in = copyToDevice<int>(
    h_in,
    NUM_VALS,
    CL_MEM_READ_ONLY);

  int h_result = 0;
  cl::Buffer* d_result = copyToDevice<int>(
    &h_result,
    1,
    CL_MEM_WRITE_ONLY);

  cl::NDRange global(1);
  cl::NDRange local(1);
  reduce_sum(cl::EnqueueArgs(*engpar_ocl_queue, global, local),
      *d_in, NUM_VALS, *d_result);
  copyFromDevice<int>(d_result, &h_result, 1);

  printf("kernel result %d\n", h_result);
  int ref = reduce_sum_gold(h_in,NUM_VALS);
  if(ref != h_result) {
    fprintf(stderr, "kernel result does not match reference result..."
        "%d kernel vs %d reference\n", h_result, ref);
    return 1;
  }

  delete[] h_in;
  delete d_in;
  delete d_result;
  delete program;

  delete engpar_ocl_device;
  delete engpar_ocl_context;
  delete engpar_ocl_queue;
  return 0;
}
