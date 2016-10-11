/*
-- File : tmcu_irq.h
--
-- Contents : functions used to control interrupts on the Target Tmcu core.
--
-- Copyright (c) 20113 by Target Compiler Technologies N.V.  All rights
-- reserved.  This file contains proprietary and confidential information of
-- Target Compiler Technologies N.V.  It may be used and modified fse.h	or
-- internal use by Target Compiler Technologies' customers under the
-- conditions of a license agreement permitting such use and modification,
-- provided this note is retained.  Any other use or disclosure requires the
-- explicit consent of Target Compiler Technologies.
*/
#ifndef __TMCU_IRQ_H__
#define __TMCU_IRQ_H__

void enable_interrupts(void);
void disable_interrupts(void);
int  get_interrupt_mask(void); 
void set_interrupt_mask(unsigned int mask);
void set_interrupt_unmask(unsigned int mask);
void core_halt(void);
void software_interrupt(int vector_addr);

#if 0
inline assembly void enable_interrupts()
    clobbers() 
    property(volatile functional loop_free)
{
    asm_begin
        ei
    asm_end
}

inline assembly void disable_interrupts()
    clobbers() 
    property(volatile functional loop_free)
{
    asm_begin
        di
    asm_end
}

extern volatile unsigned int chess_storage(IM) _imsk_var_;

inline void set_interrupt_mask(unsigned int i) { _imsk_var_ = i; }

inline assembly int get_interrupt_mask()  
    clobbers() 
    property(volatile functional loop_free)
{
    asm_begin
        mv r5, im;
        li r11, 2040;
        sw r5, (r11,0);
    asm_end
}

inline assembly void core_halt()
    clobbers() property(volatile functional loop_free)
{
    asm_begin
        halt
    asm_end
}


#define software_interrupt(x) software_interrupt_(4*x)  /* x = 1..32 */

inline assembly void software_interrupt_(int chess_storage() vector_addr)
    clobbers() property(volatile functional loop_free)
{
    asm_begin
        swi vector_addr
    asm_end
}
#endif

#endif /* __TMCU_IRQ_H_ */
