# oclsandbox
opencl sandbox

## Dependencies

- OpenCL 1.0
- CMake 2.8
- Altera OpenCL SDK 16

## Build and run on Chameleon cloud FPGA in emulated mode

More info on the Chameleon Cloud FPGA nodes is here:
https://chameleoncloud.readthedocs.io/en/latest/technical/fpga.html

### Environment Setup

```
export ALTERAOCLSDKROOT=/opt/altera_pro/16.0.2/hld
export AOCL_BOARD_PACKAGE_ROOT=/opt/altera_pro/16.0.2/hld/board/nalla_pcie
export LM_LICENSE_FILE=/opt/altera_pro/16.0.2/hld/license.dat
source /opt/altera_pro/16.0.2/hld/init_opencl.sh
```

### Build

```
git clone git@github.com:cwsmith/oclsandbox
mkdir build-oclsandbox
cd !$
export CMAKE_PREFIX_PATH=/opt/altera_pro/16.0.2/hld/board/nalla_pcie/linux64/lib:\
/opt/altera_pro/16.0.2/hld/host/linux64/lib:\
/opt/altera_pro/16.0.2/hld/host/include:$CMAKE_PREFIX_PATH
cmake ../oclsandbox/ -DENABLE_OPENCL_Altera=ON -DOPENCL_HPP_DIR=../oclsandbox/opencl -DFPGA_BOARD=p385a_sch_ax115
make
```

### Run

```
cd build-oclsandbox
CL_CONTEXT_EMULATOR_DEVICE_ALTERA=1 ./reduction/reducer --device 0 --kernel ./reduction/reduce_sum.aocx
```

If all goes well you will see the following output:

```
Inputs: --device 0 --kernel ./reduction/reduce_sum.aocx 

Using OpenCL device: EmulatorDevice : Emulated Device
extension aocx
kernel result 523776
```

## Build and run on Chameleon cloud FPGA

### Build

Follow the instructions above and add `-DKERNEL_CIRCUIT=ON` to the `cmake` command.

### Run

```
cd build-oclsandbox
./reduction/reducer --device 0 --kernel ./reduction/reduce_sum.aocx
```
