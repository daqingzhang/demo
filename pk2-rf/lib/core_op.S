.globl core_irq_enable
core_irq_enable:
	csrsi mstatus,0x1
	ret

.globl core_irq_disable
core_irq_disable:
	csrci mstatus,0x1
	ret

.globl core_ecall
core_ecall:
	ecall
	ret

.globl core_get_mstatus
core_get_mstatus:
	csrr a0, mstatus
	ret

.globl core_set_mstatus
core_set_mstatus:
	csrw mstatus, a0
	ret

.globl core_get_mcpuid
core_get_mcpuid:
	csrr a0, mcpuid
	ret

.globl core_get_mpid
core_get_mpid:
	csrr a0, mimpid
	ret

.globl core_get_mhartid
core_get_mhartid:
	csrr a0, mhartid
	ret
