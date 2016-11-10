Information for PK2-RF Processor
=======================================

Memory Layout
------------------------------------------------------------------------

	-Inst SRAM	0x00000000 -> 0x00007FFF	32KB
	-Data SRAM	0x00008000 -> 0x0000BFFF	16KB

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
	24	0x60		| Reserved
	25	0x64		| Reserved
	26	0x68		| Reserved
	27	0x6C		| Reserved
	28	0x70		| UART_Rx_Interrupt
	29	0x74		| UART_Tx_Interrupt
	30	0x78		| UART_Overrun
	31	0x7C		| Watchdog_Interrupt
	32	0x80		| Reset
	33	0x84		| Illegal Instruction
	34	0x88		| ECALL instruction Executed
	35	0x8C		| LSU Error

How to build projects
------------------------------------------------------------------------

	(1) source envsetup.sh

	(2) run make command.
		a) build test project: make test
		b) build boot project: make boot
		c) build fpga project: make fpga
		d) build default project: make

How to debug rda pulp via openOCD and BusBlaster
------------------------------------------------------------------------

	(1) openocd-riscv32 -f scrpts/dp_busblaster.cfg -f scripts/rda_pulp.cfg  -d 2 -l abc.txt

	(3) telnet localhost 4444, a low level debug to, not needed for most case
		rda mww 0x35008 0xffff - release cpu from reset state
		rda mdw 0x20000 - we can read cpu dbg_ctrl register
		rda mww 0x20000 0x10000 - halt the cpu

	(4) riscv32-gdb xxx.riscv - gdb source level debugger
		 - target remote localhost:3333
		 - load #load to program to ram
		 - set pc=0x80 # set pc to reset_handler
		 - c #run the target...

How to build openOCD tools for rda pulp
------------------------------------------------------------------------
	(1) Get source code from here:
		http://build3.rdamicro.com:8808/gangchen/openocd
	(2) Read openOCD's README file for building it.

How to build GDB tools for rda pulp
------------------------------------------------------------------------
	(1) Get source code from here:
		http://build3.rdamicro.com:8808/gangchen/binutils-gdb/tree/riscv-next

	(2) build:
		- checkout to riscv-next branch
		- cd /binutils-gdb/
		- ./configure --prefix=~/usr/opt/riscv --target=riscv32-unknown-elf --disable-werror --disable-sim
		- make
		- make install
	The --prefix is used to specify the install directory of software. It can be changed as you will.

How to build GCC tools for rda pulp
------------------------------------------------------------------------

	Good Luck!
