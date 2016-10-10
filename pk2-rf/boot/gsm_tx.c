/*
-- File : test.c
-- Contents : Example program with interrupts
-- Copyright (c) 2013 by Target Compiler Technologies N.V.
*/
#include "register.h"
#include "auxiliary.h"

extern void gsm_wcdma_rf_initial(void) property(loop_free);
extern void gsm_wcdma_rf_initial_d(void) property(loop_free);
extern short gsm_agc_calib_dig_hband[12][4];
extern short gsm_agc_calib_ana_hband[12][2];
extern short gsm_agc_calib_dig_lband[12][4];
extern short gsm_agc_calib_ana_lband[12][2];

extern short rda_8850e_tcxo;// flag of tcxo or crystal pass from modem
extern short rda_8850e_logic_version;
unsigned short gsm_agc_table_analog[12][3] = {
    {0xF8,0xBE,0x88},
    {0xF8,0x9E,0x88},
    {0xF8,0x8E,0x44},
    {0xB8,0x86,0x44},
    {0xD8,0x4E,0x44},
    {0x58,0x46,0x22},
    {0x18,0x42,0x22},
    {0x88,0x22,0x22},
    {0x28,0x22,0x22},
    {0x18,0x22,0x11},
    {0x48,0x22,0x11},
    {0x08,0x02,0x11},
};
  
void gsm_rx_off(void){
    
    //item:8850E_RF_OFF
    write_register(0xfe,0x00); // GSM_Spi chaos debug
    
    short data_12h,data_7bh;
    data_12h=read_register(0x12);
    data_7bh=read_register(0x7b);
    
    write_register(0x61,0xc0); //Com_mode=11
    write_register(0x12,(data_12h&0x80));
    //write_register(0x13,0x04); //pu function down
    write_register(0x1d,0x88); //txvco_vcobuf_ibit=1000
    write_register(0x2a,0x20); //tmx_lv_mode=0
    write_register(0x2e,0x3f); //padrv_gain=111
    write_register(0x2f,0x48); //tx_grp_vreg_pad=1000
    //write_register(0xe8,0x00);
    //write_register(0x11,0x00); //GPIO set down, gaokun
    write_register(0xfe,0x01); //page=01
    //write_register(0x10,0x08); //Pu_spll_modem_interface=1
    //write_register(0x11,0x80); //Pu_spll_wadc=1
    write_register(0x10,0x00);//remove Pu_spll_modem_interface
    write_register(0x11,0x00);
    
    write_register(0xed,0x05);//clk_bb_en,clk_abb_en   
    
    write_register(0xfe,0x00); //page=00
    
    if(rda_8850e_tcxo){
        write_register(0x13,0x06);//vctcxo,0x4+pu_afc
    }else{
        write_register(0x13,0x00);//crystal
    }
    write_register(0x7b,(data_7bh&0x04)); //Digrf_en=0,Dsp_resetn_rx=0,Dsp_resetn_tx=0
    
    // =================== rewrite
    write_register(0x23,0x61);
    // ===================
    
    ThermFsm.GPathWorking = FALSE;

} 

void gsm_rx_tx_off(void){
    
    //item:8850E_RF_OFF
    write_register(0xfe,0x00); // GSM_Spi chaos debug
    
    short data_12h,data_7bh;
    data_12h=read_register(0x12);
    data_7bh=read_register(0x7b);
    
    write_register(0x61,0xc0); //Com_mode=11
    write_register(0x12,(data_12h&0x80));
    //write_register(0x13,0x04); //pu function down
    write_register(0x1d,0x88); //txvco_vcobuf_ibit=1000
    write_register(0x2a,0x20); //tmx_lv_mode=0
    write_register(0x2e,0x3f); //padrv_gain=111
    write_register(0x2f,0x48); //tx_grp_vreg_pad=1000
    write_register(0xe8,0x00);
    write_register(0x11,0x00); //GPIO set down, gaokun
    write_register(0xfe,0x01); //page=01
    //write_register(0x10,0x08); //Pu_spll_modem_interface=1
    //write_register(0x11,0x80); //Pu_spll_wadc=1
    write_register(0x10,0x00);//remove Pu_spll_modem_interface
    write_register(0x11,0x00);
    
    write_register(0xed,0x05);//clk_bb_en,clk_abb_en   
    
    write_register(0xfe,0x00); //page=00
    
    if(rda_8850e_tcxo){
        write_register(0x13,0x06);//vctcxo,0x4+pu_afc
    }else{
        write_register(0x13,0x00);//crystal
    }
    write_register(0x7b,(data_7bh&0x04)); //Digrf_en=0,Dsp_resetn_rx=0,Dsp_resetn_tx=0

    // =================== rewrite
    write_register(0x23,0x61);
    // ===================
    
    ThermFsm.GPathWorking = FALSE;

}  
  
    
extern "C" void gsm_rxon_isr() property(isr loop_levels_1){
      
    write_register(0xfe,0x00); // GSM_Spi chaos debug
    last_rf_state = 31;
    response_bb(31,3,(0xa0));
    clear_interrupt(0x00,0x80); 
    
    unsigned short m;
    unsigned short data_38h,data_4bh;
    unsigned short x;
    unsigned short data_eah,data_ebh,data_ech,data_edh;
    unsigned short a,b,result;
    unsigned short (*analog_p)[3];
    unsigned short index_ana=0;
    unsigned short data_f6h;
    short (*calib_dig_hband)[4];
    short (*calib_dig_lband)[4];
    short data_12h,data_7bh;
    
    //gsm_rx_freq();
    write_register(0xd2,0x07);
    data_38h=read_register(0x38);
    data_4bh=read_register(0x4b);
    data_eah=read_register(0xea);
    data_ebh=read_register(0xeb);
    data_ech=read_register(0xee);
    data_edh=read_register(0xef);
    data_f6h=read_register(0xf6);
    data_12h=read_register(0x12);
    data_7bh=read_register(0x7b);
    
    m=(data_38h&0x0f);
    
    if(m<12){
            index_ana = 11-m;
        }
	else{
	    index_ana = 0;
	}
    
    if(data_38h&0x20){
        
        //mixer,pga,vco dac set
        write_register(0x61,0x00); /*Com_mode=00,no need to set it,since it works with rx agc to control
                                     the digital gain at d6H,01 is set to edge,11 is set to wcdma, gaokun*/
        write_register(0x96,0x47); //rxclk_polarity=0,digrf_rx_resetn=0
        //write_register(0xb7,0x71);
        //change to get more blocking result, gaokun
        write_register(0x14,0xa4); //lna_vbit=0100         
        write_register(0x15,0x1e); //lna_bias=00
        write_register(0x17,0x00); //pga_bw_rbit=00,pga_bw_cbit=00
        write_register(0x18,0xc0); //pga mode set
        //write_register(0x1b,data_eah); //filter_tuning=1111, moved on 0925
        //write_register(0xf6,data_ebh); //bw_sel[3:0]=0000
        //write_register(0x64,0x14); //dac1_ts_mode[4:0],don't set it to wcdma mode, gaokun          
        //write_register(0x65,0x32); //dac2 set,no use for now, gaokun
        
        write_register(0xd2,0x07);
        
        write_register(0xfe,0x01); //page=01
        //write_register(0x10,0x0f); 
        write_register(0x10,0x07);//remove Pu_spll_modem_interface         
        write_register(0x11,0x00); //rxvco set
        write_register(0x13,0x00); //notch_en off
        write_register(0x18,0x10); //rxpll_cal_bit
        write_register(0x19,0x04); //rfvco_band and rfvco_lo_select        
        write_register(0xfe,0x00); //page=00
        
        //analog gain set
        analog_p = gsm_agc_table_analog;          
        write_register(0x16,((*(analog_p+index_ana))[0]));
        write_register(0x10,((*(analog_p+index_ana))[1])); //lna,pga,filter(analog) gain set
        write_register(0xfe,0x01);
        write_register(0x17,((*(analog_p+index_ana))[2])); //rxvco_current,rxvco_vcobuf_ibit
        write_register(0xfe,0x00);
        
        //analog and digital calib dc set
        write_register(0x19,gsm_agc_calib_ana_lband[index_ana][0]);
        write_register(0x1a,gsm_agc_calib_ana_lband[index_ana][1]); //set analog calib DC        
        
        calib_dig_lband = gsm_agc_calib_dig_lband;
        write_register(0x6a,((*(calib_dig_lband+index_ana))[0]));
        write_register(0x6b,((*(calib_dig_lband+index_ana))[1]));
        write_register(0x6e,((*(calib_dig_lband+index_ana))[2]));
        write_register(0x6f,((*(calib_dig_lband+index_ana))[3])); //set digital calib DC
        
        //for GMSK/8PSK different decrease dc strategy judgement
        if(data_f6h&0x80){
            write_register(0x70,0x40); //dc_rx_cal_en=0,dcc1_mode_ct=011
        }
        else{
            write_register(0x70,0x03); //dc_rx_cal_en=0,dcc1_mode_ct=011
            write_register(0x71,0x80); //dcc1_lpf_typ=1(IIR)
            write_register(0x72,0x02); //dcc1_tmr_ct=1,for 39us 1lsb for gsm
            write_register(0x73,0x88); //dcc2_bypass=1,dcc2_lpf_typ=1(IIR)
            write_register(0x74,0x50); //dcc1_bw=00000100(20*124Hz=496Hz)
            write_register(0x75,0x00);
        }
        
        write_register(0x7b,((data_7bh&0x04)|0x80)); // 16.02.24
        //power on
        write_register(0x12,((data_12h&0x80)|0x78));          
        //write_register(0x13,0x8c); //power on,pu_dac=0,if use analog interface this bit need to set 1, gaokun
        if(rda_8850e_tcxo){
            write_register(0x13,0x8e);//vctcxo,0x8c+pu_afc
        }else{
            write_register(0x13,0x8c);//crystal
        }       
        //pll reset
        write_register(0xbd,0xa8); //mdll_startup=1
        write_register(0x92,0x49); //Rxpll_cal_resetn=0
        write_register(0x4b,(data_4bh&0x7f)|0x00); //Resetn_sdm_wd=0,tx_rx_sdm_wd=0
        
        write_register(0xbd,0x28); //mdll_startup=0
        write_register(0x92,0xc9); //Rxpll_cal_resetn=1
        write_register(0x4b,(data_4bh&0x7f)|0x80); //Resetn_sdm_wd=1,tx_rx_sdm_wd=0
        
        delay_calib(1650); //delay 50us
        
        //adc,filter,dsp reset and GPIO set       
        write_register(0xb3,0xb4); //adc_rstn=1
        write_register(0xb3,0x94); //adc_rstn=0
        write_register(0x1c,0x80); //filter_rstn=0
        write_register(0x1c,0x90); //filter_rstn=1
        /*
        if(index_ana<8){ 
            write_register(0xe8,0x20); 
            write_register(0x11,0xc0); //GPIO set lna high gain   
        }else{
            write_register(0xe8,0x22); 
            write_register(0x11,0xc0); //GPIO set lna middle gain
        }
        */
        write_register(0x96,0xc7); //rxclk_polarity=0,digrf_rx_resetn=1
        
        //write_register(0x7b,0x80); //Digrf_en=1,Dsp_resetn_rx=0,Dsp_resetn_tx=0
        write_register(0x7b,((data_7bh&0x04)|0x83)); //Digrf_en=1,Dsp_resetn_rx=1,Dsp_resetn_tx=1
        
        if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
            mipi_rffe_register_write(3,0,0x10);
        }
        //dig dc offset from BB
        
        //for GMSK/8PSK different decrease dc strategy judgement
        if(data_f6h&0x80){
           a = (data_eah << 8 | data_ebh);
           b = (data_ech << 8 | data_edh);
           
           write_register(0x68,((a>>6)&0xff));
           write_register(0x69,((a<<2)&0xff));
           write_register(0x6c,((b>>6)&0xff));
           write_register(0x6d,((b<<2)&0xff));
        }
        else{
        }

    }
    
    else if(data_38h&0x10){
        
        //mixer,pga,vco dac set
        write_register(0x61,0x00); /*Com_mode=00,no need to set it,since it works with rx agc to control
                                     the digital gain at d6H,01 is set to edge,11 is set to wcdma, gaokun*/
        write_register(0x96,0x47); //rxclk_polarity=0,digrf_rx_resetn=0
        //write_register(0xb7,0x71);
        //change to get more blocking result, gaokun
        write_register(0x14,0x58); //lna_vbit=0100//541e
        write_register(0x15,0x5e); //lna_bias=00
        write_register(0x17,0x00); //pga_bw_rbit=00,pga_bw_cbit=00
        write_register(0x18,0xc0); //pga mode set
        //write_register(0x1b,data_eah); //filter_tuning=1111, moved on 0925
        //write_register(0xf6,data_ebh); //bw_sel[3:0]=0000
        //write_register(0x1b,0x0f); //filter_tuning=1111,moved at 0925
        
        //write_register(0x64,0x14); //dac1_ts_mode[4:0],don't set it to wcdma mode, gaokun         
        //write_register(0x65,0x32); //dac2 set,no use for now, gaokun
        write_register(0xd2,0x07);
        
        write_register(0xfe,0x01); //page=01
        //write_register(0x10,0x0f); 
        write_register(0x10,0x07);//remove Pu_spll_modem_interface         
        write_register(0x11,0x00); //rxvco set
        write_register(0x13,0x00); //notch_en off
        write_register(0x18,0x10); //rxpll_cal_bit
        write_register(0x19,0x18); //rfvco_band and rfvco_lo_select
        write_register(0xfe,0x00); //page=00
        
        //analog gain set
        analog_p = gsm_agc_table_analog;          
        write_register(0x16,((*(analog_p+index_ana))[0]));
        write_register(0x10,((*(analog_p+index_ana))[1])); //lna,pga,filter gain set
        write_register(0xfe,0x01);
        write_register(0x17,((*(analog_p+index_ana))[2])); //rxvco_current,rxvco_vcobuf_ibit
        write_register(0xfe,0x00);
        
        ////analog and digital calib dc set
        write_register(0x19,gsm_agc_calib_ana_hband[index_ana][0]);
        write_register(0x1a,gsm_agc_calib_ana_hband[index_ana][1]); //set analog calib DC
        
        calib_dig_hband = gsm_agc_calib_dig_hband;
        write_register(0x6a,((*(calib_dig_hband+index_ana))[0]));
        write_register(0x6b,((*(calib_dig_hband+index_ana))[1]));
        write_register(0x6e,((*(calib_dig_hband+index_ana))[2]));
        write_register(0x6f,((*(calib_dig_hband+index_ana))[3])); //set digital calib DC
        
        //for GMSK/8PSK different decrease dc strategy judgement
        if(data_f6h&0x80){
            write_register(0x70,0x40); //dc_rx_cal_en=0,dcc1_mode_ct=011
        }
        else{
            write_register(0x70,0x03); //dc_rx_cal_en=0,dcc1_mode_ct=011
            write_register(0x71,0x80); //dcc1_lpf_typ=1(IIR)
            write_register(0x72,0x02); //dcc1_tmr_ct=1,for 39us 1lsb for gsm
            write_register(0x73,0x88); //dcc2_bypass=1,dcc2_lpf_typ=1(IIR)
            write_register(0x74,0x50); //dcc1_bw=00000100(20*124Hz=496Hz)
            write_register(0x75,0x00);
        }
        
        write_register(0x7b,((data_7bh&0x04)|0x80)); // 16.02.24
        //power on
        write_register(0x12,((data_12h&0x80)|0x78));          
        //write_register(0x13,0x8c); //power on,pu_dac=0,if use analog interface this bit need to set 1, gaokun
        if(rda_8850e_tcxo){
            write_register(0x13,0x8e);//vctcxo,0x8c+pu_afc
        }else{
            write_register(0x13,0x8c);//crystal  
        }         
        //pll reset
        write_register(0xbd,0xa8); //mdll_startup=1
        write_register(0x92,0x49); //Rxpll_cal_resetn=0
        write_register(0x4b,(data_4bh&0x7f)|0x00); //Resetn_sdm_wd=0,tx_rx_sdm_wd=0
        
        write_register(0xbd,0x28); //mdll_startup=0
        write_register(0x92,0xc9); //Rxpll_cal_resetn=1
        write_register(0x4b,(data_4bh&0x7f)|0x80); //Resetn_sdm_wd=1,tx_rx_sdm_wd=0
        
        delay_calib(1650); //delay 50us
        
        //adc,filter,dsp reset and GPIO set
        write_register(0xb3,0xb4); //adc_rstn=1
        write_register(0xb3,0x94); //adc_rstn=0
        write_register(0x1c,0x80); //filter_rstn=0
        write_register(0x1c,0x90); //filter_rstn=1
        /*
        if(index_ana<8){ 
            write_register(0xe8,0x09); 
            write_register(0x11,0x00); //GPIO set lna high gain   
        }else{
            write_register(0xe8,0x0b); 
            write_register(0x11,0x00); //GPIO set lna middle gain
        }
        */
        write_register(0x96,0xc7); //rxclk_polarity=0,digrf_rx_resetn=1
        
        //write_register(0x7b,0x80); //Digrf_en=1,Dsp_resetn_rx=0,Dsp_resetn_tx=0
        write_register(0x7b,((data_7bh&0x04)|0x83)); //Digrf_en=1,Dsp_resetn_rx=1,Dsp_resetn_tx=1
        
        if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
            mipi_rffe_register_write(3,0,0x40);
        }
        //dig dc offset from BB
        
        //for GMSK/8PSK different decrease dc strategy judgement
        if(data_f6h&0x80){ 
           a = (data_eah << 8 | data_ebh);
           b = (data_ech << 8 | data_edh);
           
           write_register(0x68,((a>>6)&0xff));
           write_register(0x69,((a<<2)&0xff));
           write_register(0x6c,((b>>6)&0xff));
           write_register(0x6d,((b<<2)&0xff));
        }
        else{
        }
       
            //response_bb(31,3);
    }
    write_register(0xfe,0x01); //GSM_Spi chaos debug
    write_register(0x1a,0x10);
    write_register(0x1b,0x88);
    write_register(0xfe,0x00);
    
    ThermFsm.GPathWorking = TRUE;
    response_bb(31,3,(0x40));//0xfb
}
    
extern "C" void gsm_rxoff_isr() property(isr) 
{
    last_rf_state = 30;
    response_bb(30,3,(0xa0));
    clear_interrupt(0x00,0x40); 
       
    //gsm_rx_tx_off();
    gsm_rx_off();
    
    therm_value_update();//therm value update 00 when g modem sleep[00-01]
    
    response_bb(30,3,(0x40));//0xf3
} 


extern "C" void gsm_txon_isr() property(isr loop_levels_1)
{

    write_register(0xfe,0x00); // GSM_Spi chaos debug
    last_rf_state = 29;
    response_bb(29,3,(0xa0));
    clear_interrupt(0x00,0x20);
     
    unsigned short data_38h,data_39h;
    unsigned short data_41h,data_42h;
    unsigned short data_cch,data_cdh;
    unsigned short m;
    short data_12h,data_7bh;
    
    //gsm_tx_freq();
    
    data_38h=read_register(0x38);
    data_39h=read_register(0x39);
    data_41h=read_register(0x41);
    data_42h=read_register(0x42);
    data_cch=read_register(0xcc);
    data_cdh=read_register(0xcd);
    data_12h=read_register(0x12);
    data_7bh=read_register(0x7b);
    
    m=(data_41h&0x03);
    
    write_register(0xfe,0x01); //page=01
    switch(m)
    {
    case 2:
    write_register(0x66,0xda);
    write_register(0x67,0x12); //mdll=9
    break;
    
    case 0:
    write_register(0x66,0xcf);
    write_register(0x67,0x3b); //mdll=7
    break;
    
    case 1:
    write_register(0x66,0xd5);
    write_register(0x67,0x54); //mdll=8
    break;

    default:
    write_register(0x66,0xdd);
    write_register(0x67,0xdd); //mdll=10
    break;
    }
    write_register(0xfe,0x00); //page=00
    
    write_register(0x20,0x88);
    write_register(0x21,0x81); //add to fix reg_20h be rewritten, 20160401 by gaokun
   
    if(data_38h&0x20){
        write_register(0x42,(data_42h&0x0f)|0x20); //freq_former_shift set,must set in GMSK TX,
                                                   //it's the ramp power level, gaokun
        
        write_register(0x61,0x00); //Com_mode=00,no need to set it,since it works with rx agc to control
                                     //the digital gain at d6H,01 is set to edge,11 is set to wcdma, gaokun
        //write_register(0x64,0x14); //dac1_ts_mode[4:0],don't set it to wcdma mode, gaokun
        //write_register(0xb7,0x71); //dac_mux_en=0,for_tx=1
        //write_register(0x2e,0x37); //tmx_gain=110,it can set by modem, gaokun
        write_register(0x1d,0x82); //txvco_vcobuf_ibit=0010
        write_register(0x1f,0x08); //rfvco_band and rfvco_lo select
        write_register(0x29,0x88); //tx_lpf set
        write_register(0x2a,0x60); //tmx_lv_mode=1
        write_register(0x2d,0x45); //tx_lpf gain set
        write_register(0x2e,0x3e); //padrv_gain=110
        
        write_register(0x96,0x2f); //txclk_polarity set,digrf_rx_resetn=0
        //write_register(0xda,0x05); //iq_swap set
        
        write_register(0x12,((data_12h&0x80)|0x07)); 
        //write_register(0x13,0x0c); //power on,pu_tx_grp=0,pu_dac_ramp=0,pu_dac=0
        if(rda_8850e_tcxo){
            write_register(0x13,0x0e);//vctcxo,0x0c+pu_afc
        }else{
            write_register(0x13,0x0c);//crystal 
        }          
        //pll reset
        write_register(0xbd,0xa8); //mdll_startup=1    
        write_register(0x39,(data_39h&0xfe)); //Txpll_cal_resetn=0
        write_register(0x41,(data_41h&0x3f)|0x40); //Resetn_sdm_gsm=0,Tx_Rx_sdm_gsm=1
        write_register(0xbd,0x28); //mdll_startup=0
        write_register(0x39,(data_39h&0xff)|0x01); //Txpll_cal_resetn=1   
        write_register(0x41,(data_41h&0x3f)|0xc0); //Resetn_sdm_gsm=1,Tx_Rx_sdm_gsm=1
        
        delay_calib(2640); //delay 80us
        
        //reset
        write_register(0x2c,0x78); //tmx_op_rstn=0,tx_grp_pll_test=1
        write_register(0x2c,0xf8); //tmx_op_rstn=1,tx_grp_pll_test=1
        
        
        write_register(0x96,0xaf); //txclk_polarity set,digrf_rx_resetn=1
        
        //write_register(0x7b,0x00); //Dsp_resetn_rx=0,Dsp_resetn_tx=0
        write_register(0x7b,((data_7bh&0x04)|0x03)); //Dsp_resetn_rx=1,Dsp_resetn_tx=1
        
        write_register(0xe8,data_cch);
        write_register(0x11,data_cdh); //GPIO set
        
        if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
            mipi_rffe_register_write(3,0,0x01);
        }

        delay_calib(165); //delay 5us
        
        if(rda_8850e_tcxo){
            write_register(0x13,0x1e);//vctcxo,0x1c+pu_afc
        }else{
            write_register(0x13,0x1c);//crystal  
        }
	 //write_register(0x13,0x1c); //power on,pu_tx_grp=1,pu_dac_ramp=0

	 delay_calib(33); //delay 1us
	 
        if(rda_8850e_tcxo){
            write_register(0x13,0x3e);//vctcxo,0x0c+pu_afc
        }else{
            write_register(0x13,0x3c);//crystal  
        }	
        //write_register(0x13,0x3c); //power on,pu_tx_grp=1,pu_dac_ramp=1,pu_dac=0,if use analog interface this bit need to set 1, gaokun       
        
        //write_register(0xf0,0x06); 
        //write_register(0xf1,0x34); //ramp set,concern if it's necessary, gaokun
        
        //write_register(0xfe,0x01); //page=01
        //write_register(0x13,0x0c); //Tia_gain set,concern if it's necessary, gaokun
        //write_register(0xfe,0x00); //page=00
        
        //write_register(0x32,0x00); //need to set the GMSK TX ramp power level, gaokun
       
    }
    else if(data_38h&0x10){
        write_register(0x42,(data_42h&0x0f)|0x00); //freq_former_shift set,must set in GMSK TX,
                                                     //it's the ramp power level, gaokun
        
        write_register(0x61,0x00); //Com_mode=00,no need to set it,since it works with rx agc to control
                                     //the digital gain at d6H,01 is set to edge,11 is set to wcdma, gaokun
        //write_register(0x64,0x14); //dac1_ts_mode[4:0],don't set it to wcdma mode, gaokun
        //write_register(0xb7,0x71); //dac_mux_en=0,for_tx=1
        //write_register(0x2e,0x37); //tmx_gain=110,it can set by modem, gaokun
        write_register(0x1d,0x82); //txvco_vcobuf_ibit=0010
        write_register(0x1f,0x94); //rfvco_band and rfvco_lo select
        write_register(0x29,0x88); //tx_lpf set
        write_register(0x2a,0x60); //tmx_lv_mode=1
        write_register(0x2d,0x45); //tx_lpf gain set
        write_register(0x2e,0x3e); //padrv_gain=110
        
        write_register(0x96,0x2f); //txclk_polarity set,digrf_rx_resetn=0
        //write_register(0xda,0x05); //iq_swap set
        
        write_register(0x12,((data_12h&0x80)|0x07)); 
        //write_register(0x13,0x0c); //power on,pu_tx_grp=0,pu_dac_ramp=0,pu_dac=0
        if(rda_8850e_tcxo){
            write_register(0x13,0x0e);//vctcxo,0x0c+pu_afc
        }else{
            write_register(0x13,0x0c);//crystal    
        }      
        //pll reset
        write_register(0xbd,0xa8); //mdll_startup=1    
        write_register(0x39,(data_39h&0xfe)); //Txpll_cal_resetn=0
        write_register(0x41,(data_41h&0x3f)|0x40); //Resetn_sdm_gsm=0,Tx_Rx_sdm_gsm=1
        write_register(0xbd,0x28); //mdll_startup=0
        write_register(0x39,(data_39h&0xff)|0x01); //Txpll_cal_resetn=1   
        write_register(0x41,(data_41h&0x3f)|0xc0); //Resetn_sdm_gsm=1,Tx_Rx_sdm_gsm=1

        delay_calib(2640); //delay 80us
        
        //reset
        write_register(0x2c,0x78); //tmx_op_rstn=0,tx_grp_pll_test=1
        write_register(0x2c,0xf8); //tmx_op_rstn=1,tx_grp_pll_test=1
        
        
        write_register(0x96,0xaf); //txclk_polarity set,digrf_rx_resetn=1
        
        //write_register(0x7b,0x00); //Dsp_resetn_rx=0,Dsp_resetn_tx=0
        write_register(0x7b,((data_7bh&0x04)|0x03)); //Dsp_resetn_rx=1,Dsp_resetn_tx=1
        
        write_register(0xe8,data_cch);
        write_register(0x11,data_cdh); //GPIO set
        
        if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
            mipi_rffe_register_write(3,0,0x04);
        }

        delay_calib(165); //delay 5us
        
        if(rda_8850e_tcxo){
            write_register(0x13,0x1e);//vctcxo,0x1c+pu_afc
        }else{
            write_register(0x13,0x1c);//crystal  
        }
	 //write_register(0x13,0x1c); //power on,pu_tx_grp=1,pu_dac_ramp=0

	delay_calib(33); //delay 1us
	
        if(rda_8850e_tcxo){
            write_register(0x13,0x3e);//vctcxo,0x3c+pu_afc
        }else{
            write_register(0x13,0x3c);//crystal  
        }	
        //write_register(0x13,0x3c); //power on,pu_tx_grp=1,pu_dac_ramp=1,pu_dac=0,if use analog interface this bit need to set 1, gaokun
        	 
        //write_register(0xf0,0x06); 
        //write_register(0xf1,0x34); //ramp set,concern if it's necessary, gaokun

        //write_register(0xfe,0x01); //page=01
        //write_register(0x13,0x0c); //Tia_gain set,concern if it's necessary, gaokun
        //write_register(0xfe,0x00); //page=00
        
        //write_register(0x32,0x00); //need to set the GMSK TX ramp power level, gaokun     
    } 
    
    ThermFsm.GPathWorking = TRUE;
    //therm_internal_trigger();
    response_bb(29,3,(0x40));//0xeb
}
    
extern "C" void gsm_txoff_isr() property(isr) 
{
    last_rf_state = 28;
    response_bb(28,3,(0xa0));
    clear_interrupt(0x00,0x10); 
    
    //therm_value_update();  
    
    gsm_rx_tx_off();
    response_bb(28,3,(0x40));//0xe3
     
}       

extern "C" void gsm_initial_isr() property(isr) 
{
    response_bb(23,3,(0xa0));
    clear_interrupt(0x01,0x80);
        
    gsm_wcdma_rf_initial();    
    
    response_bb(23,3,(0x40));//0xbb
} 


extern "C" void gsm_initial_d_isr() property(isr) 
{
    response_bb(22,3,(0xa0));
    clear_interrupt(0x01,0x40);
        
    gsm_wcdma_rf_initial_d();    
    
    response_bb(22,3,(0x40));//0xbb
} 



