#ifndef __CORE_OP_H__
#define __CORE_OP_H__

void core_irq_enable(void);
void core_irq_disable(void);
void core_ecall(void);
void core_set_mstatus(unsigned int status);
unsigned int core_get_mstatus(void);
unsigned int core_get_mcpuid(void);
unsigned int core_get_mpid(void);
unsigned int core_get_mhartid(void);

#endif
