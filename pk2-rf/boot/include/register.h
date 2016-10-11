//-------------------------------
// register releated stuff
//-------------------------------

#ifndef __REGISTER_H__
#define __REGISTER_H__

#if 0
extern int read_register(int addr) property(loop_free);
extern void write_register(int addr,int data)  property(loop_free);
extern void clear_interrupt(const int addr,const int value) property(loop_free);
extern void response_bb(const int index,const int code,const int location) property(loop_free);
extern void hw_int_accum(const short index) property(loop_free);
extern volatile signed int *WADDR_EXT_REG;
#endif
extern int  read_register(int addr);
extern void write_register(int addr,int data);
extern void clear_interrupt(const int addr,const int value);
extern void response_bb(const int index,const int code,const int location);
extern void hw_int_accum(const short index);
extern volatile signed int *WADDR_EXT_REG;

#if 0
// extern function definition
inline assembly void delay_1us(void)//1us
    clobbers() 
    property(volatile functional loop_free)
{
    asm_begin
    .lrepeat 10
             nop
    asm_end
}

inline assembly void delay(int chess_storage() repeat_num) property(volatile loop_free) clobbers()//27=1us
{
    asm_begin
    do repeat_num,1;
    nop;
    nop;
    nop;
    nop;
    asm_end
}

inline assembly void delay_n(int chess_storage() repeat_num) property(volatile loop_free) clobbers(r0)//27=1us
{
    asm_begin
    do repeat_num,1;
    li r0,0;
    li r0,0;
    li r0,0;
    nop;
    asm_end
}

inline assembly void delay_calib(int chess_storage(r1) repeat_num) property(volatile loop_free) clobbers(r0,r1,r2)
{
    asm_begin
    li r0,0;
    sub r1,r1,1;
    sges r2,r1,r0;
    jnez r2,-4;
    nop;
    nop;
    asm_end
}
#endif

void delay_1us(void);
void delay(int us);
void delay_n(int us);
void delay_calib(int us);

#endif /* __REGISTER_H__ */
