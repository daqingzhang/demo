/*
-- File : init.c
-- Contents : - initialisation of stack pointer and interrupts 
-- Copyright (c) 2013 by Target Compiler Technologies N.V.
*/

#include <config.h>
#include <util.h>

//#define ISR_MARK_ADDR 0x00013000

void gsm_rxon_isr(void)		;//  writel(0x31,ISR_MARK_ADDR); }	//31
void gsm_rxoff_isr(void)	;//  writel(0x30,ISR_MARK_ADDR); }	//30
void gsm_txon_isr(void)		;//  writel(0x29,ISR_MARK_ADDR); }	//29
void gsm_txoff_isr(void)	;//  writel(0x28,ISR_MARK_ADDR); }	//28
void gsm_initial_isr(void)	;//  writel(0x23,ISR_MARK_ADDR); }	//23
void gsm_initial_d_isr(void)	;//  writel(0x22,ISR_MARK_ADDR); }	//22
void gsm_dc_calib_isr(void)	;//  writel(0x21,ISR_MARK_ADDR); }	//21
void irat_dc_calib_isr(void)	;//  writel(0x20,ISR_MARK_ADDR); }	//20
void gsm_therm_on_isr(void)	;//  writel(0x19,ISR_MARK_ADDR); }	//19
void gsm_therm_off_isr(void)	;//  writel(0x18,ISR_MARK_ADDR); }	//18
void gsm_therm_update_isr(void)	;//  writel(0x17,ISR_MARK_ADDR); }	//17
void wcdma_rx_freq_hw_isr(void)	;//  writel(0x15,ISR_MARK_ADDR); }	//15
void wcdma_rx_gain_hw_isr(void)	;//  writel(0x14,ISR_MARK_ADDR); }	//14
void wcdma_tx_freq_hw_isr(void)	;//  writel(0x13,ISR_MARK_ADDR); }	//13
void wcdma_tx_pwr_hw_isr(void)	;//  writel(0x12,ISR_MARK_ADDR); }	//12
void wcdma_therm_hw_isr(void)	;//  writel(0x11,ISR_MARK_ADDR); }	//11
void wcdma_irat_hw_isr(void)	;//  writel(0x09,ISR_MARK_ADDR); }	//09
void wcdma_spll_hw_isr(void)	;//  writel(0x08,ISR_MARK_ADDR); }	//08
void wcdma_rf_initial_isr(void)	;//  writel(0x07,ISR_MARK_ADDR); }	//07
void wcdma_rf_on_isr(void)	;//  writel(0x06,ISR_MARK_ADDR); }	//06
void wcdma_rf_off_isr(void)	;//  writel(0x05,ISR_MARK_ADDR); }	//05
void wcdma_tx_on_isr(void)	;//  writel(0x04,ISR_MARK_ADDR); }	//04
void wcdma_tx_off_isr(void)	;//  writel(0x03,ISR_MARK_ADDR); }	//03
void wcdma_rx_calib_isr(void)	;//  writel(0x02,ISR_MARK_ADDR); }	//02
void wcdma_tx_calib_isr(void)	;//  writel(0x01,ISR_MARK_ADDR); }	//01

void dispatch_isr(unsigned int status)
{
	if(status & IRQ_MASK_REG00_WR) {
		wcdma_tx_calib_isr();	//01
	}
	if(status & IRQ_MASK_REG02_WR) {
		wcdma_rx_calib_isr();	//02
	}
	if(status & IRQ_MASK_REG04_WR) {
		wcdma_tx_off_isr();	//03
	}
	if(status & IRQ_MASK_REG06_WR) {
		wcdma_tx_on_isr();	//04
	}
	if(status & IRQ_MASK_TIMER0_UDF) {
		nop();
	}
	if(status & IRQ_MASK_TIMER1_UDF) {
		nop();
	}
	if(status & IRQ_MASK_TIMER2_UDF) {
		nop();
	}
	if(status & IRQ_MASK_AON) {
		nop();
	}
	if(status & IRQ_MASK_REG10_WR) {
		wcdma_rf_off_isr();	//05
	}
	if(status & IRQ_MASK_REG12_WR) {
		wcdma_rf_on_isr();	//06
	}
	if(status & IRQ_MASK_REG14_WR) {
		wcdma_rf_initial_isr();	//07
	}
	if(status & IRQ_MASK_REG16_WR) {
		wcdma_spll_hw_isr();	//08
	}
	if(status & IRQ_MASK_REG18_WR) {
		wcdma_irat_hw_isr();	//09
	}
	if(status & IRQ_MASK_REG1A_WR) {
		wcdma_tx_pwr_hw_isr();	//12
	}
	if(status & IRQ_MASK_REG1C_WR) {
		wcdma_tx_freq_hw_isr();	//13
	}
	if(status & IRQ_MASK_REG1E_WR) {
		wcdma_therm_hw_isr();	//11
	}
	if(status & IRQ_MASK_REG20_WR) {
		wcdma_rx_gain_hw_isr();	//14
	}
	if(status & IRQ_MASK_REG22_WR) {
		wcdma_rx_freq_hw_isr();	//15
	}
	if(status & IRQ_MASK_REG24_WR) {
		gsm_therm_update_isr();	//17
	}
	if(status & IRQ_MASK_REG26_WR) {
		gsm_therm_off_isr();	//18
	}
	if(status & IRQ_MASK_REG28_WR) {
		gsm_therm_on_isr();	//19
	}
	if(status & IRQ_MASK_REG2A_WR) {
		irat_dc_calib_isr();	//20
	}
	if(status & IRQ_MASK_REG2C_WR) {
		gsm_dc_calib_isr();	//21
	}
	if(status & IRQ_MASK_REG2E_WR) {
		gsm_initial_d_isr();	//22
	}
	if(status & IRQ_MASK_RSV1) {
		gsm_initial_isr();	//23
	}
	if(status & IRQ_MASK_RSV2) {
		gsm_txoff_isr();	//28
	}
	if(status & IRQ_MASK_RSV3) {
		gsm_txon_isr();		//29
	}
	if(status & IRQ_MASK_RSV4) {
		gsm_rxoff_isr();	//30
		gsm_rxon_isr();		//31
	}
	if(status & IRQ_MASK_UART_RX_DONE) {
		nop();
	}
	if(status & IRQ_MASK_UART_TX_DONE) {
		nop();
	}
	if(status & IRQ_MASK_UART_OVR) {
		nop();
	}
	if(status & IRQ_MASK_WDOG_UDF) {
		nop();
	}
}
