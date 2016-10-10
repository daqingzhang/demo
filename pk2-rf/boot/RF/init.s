/*
-- File : testisr_init.s
-- Contents : - initialisation of stack pointer and interrupts 
-- Copyright (c) 2013 by Target Compiler Technologies N.V.
*/

.undef global text _main

.undef global text gsm_rxon_isr            //31
.undef global text gsm_rxoff_isr           //30
.undef global text gsm_txon_isr            //29
.undef global text gsm_txoff_isr           //28

.undef global text gsm_initial_isr         //23
.undef global text gsm_initial_d_isr       //22
.undef global text gsm_dc_calib_isr        //21
.undef global text irat_dc_calib_isr       //20
.undef global text gsm_therm_on_isr        //19
.undef global text gsm_therm_off_isr       //18
.undef global text gsm_therm_update_isr    //17

.undef global text wcdma_rx_freq_hw_isr    //15
.undef global text wcdma_rx_gain_hw_isr    //14
.undef global text wcdma_tx_freq_hw_isr    //13
.undef global text wcdma_tx_pwr_hw_isr     //12
.undef global text wcdma_therm_hw_isr      //11
.undef global text wcdma_irat_hw_isr       //09
.undef global text wcdma_spll_hw_isr       //08

.undef global text wcdma_rf_initial_isr    //07
.undef global text wcdma_rf_on_isr         //06
.undef global text wcdma_rf_off_isr        //05
.undef global text wcdma_tx_on_isr         //04
.undef global text wcdma_tx_off_isr        //03
.undef global text wcdma_rx_calib_isr      //02
.undef global text wcdma_tx_calib_isr

;; the interrupt vector table with 4 interrupts
.text global 0 _ivt
        j.f isr_init;  reset (interrupt 0)
        nop 
	nop

        j.f wcdma_tx_calib_isr             ;  interrupt 1 
        nop 
	nop

        j.f wcdma_rx_calib_isr          ;  interrupt 2
        nop  
        nop 

        j.f wcdma_tx_off_isr                 ;  interrupt 3
        nop 
	nop

        j.f wcdma_tx_on_isr               ;  interrupt 4
        nop
	nop

        j.f wcdma_rf_off_isr              ;  interrupt 5
        nop 
	nop

        j.f wcdma_rf_on_isr              ;  interrupt 6
        nop  
        nop 

        j.f wcdma_rf_initial_isr         ;  interrupt 7
        nop 
	nop

        j.f wcdma_spll_hw_isr              ;  interrupt 8
        nop
	nop

        j.f wcdma_irat_hw_isr              ;  interrupt 9
        nop 
	nop

        nop          ;  interrupt 10
        nop
        nop  
        nop 

        j.f wcdma_therm_hw_isr          ;  interrupt 11
        nop 
	nop

        j.f wcdma_tx_pwr_hw_isr              ;  interrupt 12
        nop
	nop

        j.f wcdma_tx_freq_hw_isr              ;  interrupt 13
        nop 
	nop

        j.f wcdma_rx_gain_hw_isr              ;  interrupt 14
        nop  
        nop 

        j.f wcdma_rx_freq_hw_isr          ;  interrupt 15
        nop 
	nop

        nop              ;  interrupt 16 
        nop
        nop
	nop
	
        j.f gsm_therm_update_isr              ;  interrupt 17
        nop 
	nop

        j.f gsm_therm_off_isr          ;  interrupt 18
        nop  
        nop 

        j.f gsm_therm_on_isr         ;  interrupt 19
        nop 
	nop

        j.f irat_dc_calib_isr              ;  interrupt 20 
        nop
	nop

        j.f gsm_dc_calib_isr              ;  interrupt 21
        nop 
	nop

        //j.f gsm_initial_d_isr
        nop
        nop                         ;  interrupt 22
        nop  
        nop 

        j.f gsm_initial_isr          ;  interrupt 23
        nop 
	nop

        nop              ;  interrupt 24
        nop 
        nop
	nop

        nop              ;  interrupt 25
        nop 
        nop 
	nop

        nop         ;  interrupt 26
        nop  
        nop
        nop

        nop         ;  interrupt 27
        nop 
	nop
	nop

        j.f gsm_txoff_isr              ;  interrupt 28
        nop
	nop

        j.f gsm_txon_isr              ;  interrupt 29
        nop 
	nop

        j.f gsm_rxoff_isr              ;  interrupt 30
        nop  
        nop 

        j.f gsm_rxon_isr         ;  interrupt 31
        nop
	nop

; initialize SP
.text global 0 isr_init
	li sp, _sp_start_value_DMb
        li lr, 0     ; reset for HDL simulation
	mv im, r1    ; enable interrupt 2
	nop
	j _main
	nop
