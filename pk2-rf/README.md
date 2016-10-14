Information for PK2-RF Processor
=======================================

Memory Layout
------------------------------------------------------------------------

	-Inst SRAM	0x00000000 -> 0x00003FFF	16KB
	-Data SRAM	0x00010000 -> 0x00013FFF	16KB

Vector Table
------------------------------------------------------------------------
	No.	Addr		Interrupt Source

	0	0x00		| APB register 0x30000
	1	0x04		| APB register 0x30002
	2	0x08		| APB register 0x30004
	3	0x0C		| APB register 0x30006
	4	0x10		| TIMER0
	5	0x14		| TIMER1
	6	0x18		| TIMER2
	7	0x1C		| AON
	8	0x20		| APB register 0x30010
	9	0x24		| APB register 0x30012
	10	0x28		| APB register 0x30014
	11	0x2C		| APB register 0x30016
	12	0x30		| APB register 0x30018
	13	0x34		| APB register 0x3001a
	14	0x38		| APB register 0x3001c
	15	0x3C		| APB register 0x3001e
	16	0x40		| APB register 0x30020
	17	0x44		| APB register 0x30022
	18	0x48		| APB register 0x30024
	19	0x4C		| APB register 0x30026
	20	0x50		| APB register 0x30028
	21	0x54		| APB register 0x3002a
	22	0x58		| APB register 0x3002c
	23	0x5C		| APB register 0x3002e
	24	0x60		| reserved
	25	0x64		| reserved
	26	0x68		| reserved
	27	0x6C		| reserved
	28	0x70		| reserved
	29	0x74		| reserved
	30	0x78		| reserved
	31	0x7C		| reserved
	32	0x80		| Reset
	33	0x84		| Illegal Instruction
	34	0x88		| ECALL instruction Executed
	35	0x8C		| LSU Error

How to make
------------------------------------------------------------------------

	(1) Firstly, You have to add compiler directory to your PATH.
	For example, open .bashrc via VIM and modify PATH like this:
		export PATH=~/bin:$PATH:~/gcc-riscv32-pulp/bin

	(2) Please modify the Makefile's ABS_TOP_DIR varible to specify the absolutely
	file path of current project. Just write it like this:
		ABS_TOP_DIR	:= /home/zhangdaqing/workspace/iot/riscv/rda-riscv

	(3) Run Makefile
		> make

	Good Luck!
