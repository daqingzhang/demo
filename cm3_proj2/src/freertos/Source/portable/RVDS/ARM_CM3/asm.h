#ifndef __ASM_H__
#define __ASM_H__

void __dsb(int n);
void __isb(int n);

void vPortSVCHandler( void );
void prvStartFirstTask( void );
void xPortPendSVHandler( void );
uint32_t ulPortSetInterruptMask( void );
void vPortClearInterruptMask( uint32_t ulNewMask );
uint32_t vPortGetIPSR( void );

#endif
