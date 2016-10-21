RISCV_GCCBIN=~/gcc-riscv32-pulp/bin
RISCV_GCCLIB=~/workspace/github/demo/pk2-rf/gcclib/usr/lib

export LD_LIBRARY_PATH=$RISCV_GCCLIB
export PATH=$PATH:$RISCV_GCCBIN

riscvenv()
{
	echo
	echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
	echo "PATH=$PATH"
	echo
}

riscvenv
