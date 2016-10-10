Information for PK2-RF Processor
=======================================

Memory Layout
-------------------------------------------------------------------------

	-Inst SRAM	0x00000000 -> 0x00003FFF	16KB
	-Data SRAM	0x00010000 -> 0x00013FFF	16KB

Vector Table
-------------------------------------------------------------------------

	0x00 - 0x7C	| Interrupts 0 - 31
	0x80		| Reset
	0x84		| Illegal Instruction
	0x88		| ECALL instruction Executed
	0x8C		| LSU Error

How to make
-------------------------------------------------------------------------

	(1) Firstly, You have to add compiler directory to your PATH.
	For example, open .bashrc via VIM and modify PATH like this:
		export PATH=~/bin:$PATH:~/gcc-riscv32-pulp/bin

	(2) Please modify the Makefile's ABS_TOP_DIR varible to specify the absolutely
	file path of current project. Just write it like this:
		ABS_TOP_DIR	:= /home/zhangdaqing/workspace/iot/riscv/rda-riscv

	(3) Run Makefile
		> make

	Good Luck!
