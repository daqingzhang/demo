/*----------------------------------------------------------------------*/
/* Setup                                                                */
/*----------------------------------------------------------------------*/

/* The OUTPUT_ARCH command specifies the machine architecture where the
   argument is one of the names used in the BFD library. More
   specifically one of the entires in bfd/cpu-mips.c */

OUTPUT_ARCH( "riscv" )

/*----------------------------------------------------------------------*/
/* Sections                                                             */
/*----------------------------------------------------------------------*/

SECTIONS
{
	/* text segment */
	. = 0x00000000;
	.text : {
		boot/crt.o	(.text)
		boot/syscall.o	(.text)
		*(.text)
		. = ALIGN(4);
	}
	__text_end__ = . ;

	/* data segment */
	. = 0x00010000;
	__data_start__ = . ;
	.sdata : {
		/*_gp = . ;*/
		*(.srodata.cst16)
		*(.srodata.cst8)
		*(.srodata.cst4)
		*(.srodata.cst2)
		*(.srodata*)
		*(.sdata .sdata.* .gnu.linkonce.s.*)
		. = ALIGN(4);
	}
	.data : {
		*(.data)
		. = ALIGN(4);
	}
	__data_end__ = . ;

	/* bss segment */
	__bss_start__ = . ;
	.sbss : {
		*(.sbss .sbss.* .gnu.linkonce.sb.*)
		*(.scommon)
		. = ALIGN(4);
	}
	.bss : {
		*(.bss)
		. = ALIGN(4);
	}
	__bss_end__ = . ;
	/* End of uninitalized data segement */
	_end = . ;
}
