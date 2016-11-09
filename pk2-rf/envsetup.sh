RISCV_CUR_DIR=$(pwd)
RISCV_GCCBIN=$RISCV_CUR_DIR/tools/gcc-riscv32-pulp/bin
RISCV_GCCLIB=$RISCV_CUR_DIR/tools/gcclib/usr/lib

export LD_LIBRARY_PATH=$RISCV_GCCLIB
export PATH=$PATH:$RISCV_GCCBIN

riscvenv()
{
	echo
	echo "RISCV_CUR_DIR=$RISCV_CUR_DIR"
	echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
	echo "PATH=$PATH"
	echo
}

riscvenv
