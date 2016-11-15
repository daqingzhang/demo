RISCV_CUR_DIR=$(pwd)
RISCV_GCCLIB_DIR=$RISCV_CUR_DIR/pk2-tools/gcclib/usr/lib
RISCV_GCC_DIR=$RISCV_CUR_DIR/pk2-tools/gcc-riscv32-pulp/bin
RISCV_GDB_DIR=$RISCV_CUR_DIR/pk2-tools/gdb-riscv32-pulp/bin
RISCV_OPENOCD_DIR=$RISCV_CUR_DIR/pk2-tools/openocd-pulp/bin

export LD_LIBRARY_PATH=$RISCV_GCCLIB_DIR
export PATH=$PATH:$RISCV_GCC_DIR:$RISCV_GDB_DIR:$RISCV_OPENOCD_DIR

riscvenv()
{
	echo
	echo "RISCV_CUR_DIR=$RISCV_CUR_DIR"
	echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
	echo "PATH=$PATH"
	echo
}

riscvenv
