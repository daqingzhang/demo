OUTPUT_FORMAT("elf32-littlearm","elf32-littlearm","elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
	ram (rwx) : 	o = 0x20000000, l = 20K
	flash (rx) : 	o = 0x08000000, l = 126K
}

_estack = ORIGIN(ram) + LENGTH(ram);
ASSERT(ORIGIN(flash) == ALIGN(ORIGIN(flash), 0x80), "start of flash memory not aligned");

SECTIONS
{
	.text : {
		. = ALIGN(0x80);
		startup.o
		. = ALIGN(0x80);
		sum.o
	} > flash
	.rodata : {
		. = ALIGN(0x4);
		*(.rodata*)
		. = ALIGN(0x4);
	} > flash

	.data : {
		__data_start = .;
		*(.data)
		__data_end = .;
	} > ram

	.bss (NOLOAD) : {
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	} > ram

	/DISCARD/ : { *(.gnu*) }
}
