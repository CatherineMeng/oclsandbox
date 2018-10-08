source /home/cwsmith/software/spack_install/linux-centos7-x86_64/gcc-centos_7.2/lmod-7.8-rfoosrn6l7ttojvsumrgowujbwlkqnl4/lmod/lmod/init/bash
unset MODULEPATH
module use /home/cwsmith/software/spack_install/lmod/linux-centos7-x86_64/Core

module load gcc cmake

export ALTERAOCLSDKROOT=/opt/altera_pro/16.0.2/hld
export AOCL_BOARD_PACKAGE_ROOT=/opt/altera_pro/16.0.2/hld/board/nalla_pcie
export LM_LICENSE_FILE=/opt/altera_pro/16.0.2/hld/license.dat
source /opt/altera_pro/16.0.2/hld/init_opencl.sh
