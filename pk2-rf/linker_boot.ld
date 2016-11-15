/*
 * Link Script For RISCV CPU
 *******************************************************************************
 */

OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
	ram0 (rx) : o = 0x00000000, l = 0x8000 /* 32KB RAM for text + rodata */
	ram1 (rw) : o = 0x00008000, l = 0x4000 /* 16KB RAM for sdata + data + sbss + bss + stack  */
}

SECTIONS
{
	/* place text in RAM0 */
	.text : {
		lib/crt_boot.o	(.text)
		lib/syscall.o	(.text)
		lib/*		(.text) /* object files for library */
		boot/*		(.text) /* object files for test */
		*(.text)		/* other object files */
		. = ALIGN(4);		/* address aligned 4 Bytes */
	} > ram0

	__text_end__ = . ;

	/* place rodata in RAM0 */
	.rodata : {
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
		. = ALIGN(4);
	} > ram0
	__ram0_end__ = . ;

	/* place sdata & data in low RAM1 */
	.sdata : {
		__data_start__ = . ;
		*(.srodata.cst16)
		*(.srodata.cst8)
		*(.srodata.cst4)
		*(.srodata.cst2)
		*(.srodata*)
		*(.sdata .sdata.* .gnu.linkonce.s.*)
		. = ALIGN(4);
	} > ram1 /*AT>ram0*/

	.data : {
		*(.data)
		__data_end__ = . ;
		. = ALIGN(4);
	} > ram1 /*AT>ram0*/

	/* place the sbss & bss data in high RAM1 */
	.sbss (NOLOAD) : {
		__bss_start__ = . ;
		*(.sbss .sbss.* .gnu.linkonce.sb.*)
		*(.scommon)
		. = ALIGN(4);
	} > ram1

	.bss (NOLOAD) : {
		*(.bss)
		. = ALIGN(4);
		__bss_end__ = . ;
	} > ram1

	_end = . ;
}

ASSERT((__ram0_end__ < (ORIGIN(ram0) + LENGTH(ram0))), "ERROR: no enough space to store code in RAM0 !");
ASSERT((_end < (ORIGIN(ram1) + LENGTH(ram1))), "ERROR: no enough space to store data in RAM1 !");
