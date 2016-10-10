//---------------
//
//
//-----------------

#include "register.h"
#include "auxiliary.h"

extern void gsm_rx_tx_off(void) property(loop_free);
extern unsigned short gsm_agc_table_analog[12][3];
extern short rda_8850e_logic_version;
extern short rda_8850e_tcxo;// flag of tcxo or crystal pass from modem

#define H_BAND 1
#define L_BAND 0

short gsm_agc_calib_ana_hband[12][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
short gsm_agc_calib_dig_hband[12][4] = {{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{33,0,0,0},{33,0,0,0},{33,0,0,0},{33,0,0,0}};
short gsm_agc_calib_ana_lband[12][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
short gsm_agc_calib_dig_lband[12][4] = {{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{33,0,0,0},{33,0,0,0},{33,0,0,0},{33,0,0,0}};

static void gsm_rx_calib_freq_hband(void){ //take rx_freq=1900MHz as example, gaokun
          
    //mdll
    write_register(0xbc,0x7c);

    //rxpll_target
    write_register(0x90,0x4a);
    write_register(0x91,0x6c);
    
    //rx_freq_1935MHz,for DCS 1805~1880MHz
            
    write_register(0x46,0x0a);
    write_register(0x47,0xa1); 
    write_register(0x48,0xc2);
    write_register(0x49,0x1c);  
}

static void gsm_rx_calib_freq_lband(void){ //take rx_freq=900MHz as example, gaokun     
    
    //mdll_div_num      
    write_register(0xbc,0x7c);

    //rxpll_target
    write_register(0x90,0x49);
    write_register(0x91,0x76);
    
    //rx_freq_955MHz,for GSM 935~960MHz      
    write_register(0x46,0x8a);
    write_register(0x47,0x7e); 
    write_register(0x48,0x97);
    write_register(0x49,0xe9);  
    
}

static void gsm_rx_calib_ana_gain(char band,short index){
    
    unsigned short (*analog_p)[3];
    /*
    int data_cch;
    int data_cdh;
    int data_ceh;
    int data_cfh;
    
    data_cch=read_register(0xcc);
    data_cdh=read_register(0xcd);
    data_ceh=read_register(0xce);
    data_cfh=read_register(0xcf);
    */  
    if(H_BAND==band){
        write_register(0x11,0x00);
        //write_register(0x11,data_cch);
        
        if(index<8){ 
            if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
                write_register(0xe8,0x2e);//0x26
                mipi_rffe_register_write(3,0,0x40);
            }else{
                write_register(0xe8,0x08);//H gain
            }
            //write_register(0xe8,data_cdh);//H gain
        }else{
            if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
                write_register(0xe8,0x2c);//0x24
                mipi_rffe_register_write(3,0,0x40);
            }else{
                write_register(0xe8,0x0a);//M gain
            }
        }
        
        //write_register(0x17,0x9e);//pga_bw_rbit=0b10
        //write_register(0x17,0x00);

    }
    else if(L_BAND==band){
        
        write_register(0x11,0x00);
        //write_register(0x11,data_ceh);
        
        if(index<8){
            if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
                write_register(0xe8,0x26);//0x2e
                mipi_rffe_register_write(3,0,0x10);
            }else{
                write_register(0xe8,0x20); 
            }
            //write_register(0xe8,data_cfh);//H gain
        }else{
            if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
                write_register(0xe8,0x24);//0x2c
                mipi_rffe_register_write(3,0,0x10);
            }else{
                write_register(0xe8,0x22);//M gain
            }
        }
        
        //write_register(0x17,0x00);//pga_bw_rbit=0b10
   
    }
    
    analog_p = gsm_agc_table_analog;
   
    write_register(0x16,((*(analog_p+index))[0]));
    write_register(0x10,((*(analog_p+index))[1]));
    write_register(0xfe,0x01);
    write_register(0x17,((*(analog_p+index))[2])); //rxvco_current,rxvco_vcobuf_ibit
    write_register(0xfe,0x00); 
}


static int dummy_read(int addr){

    unsigned short data=0;
    unsigned short y=0;
    delay_calib(330); //delay 10us
    data=read_register(addr);

    return(data);
}

static void approach(void) property(loop_levels_1){
    
    unsigned short loop_num=0;
    
    volatile unsigned short data_88h;
    volatile unsigned short data_19h;
    volatile unsigned short data_1ah;
    
    for(;loop_num<512;loop_num++){
        data_88h=dummy_read(0x88);
        
        if((data_88h&0x80)>>7){
            data_19h=read_register(0x19);
            write_register(0x19,(data_19h-1));
        }else{        
            data_19h=read_register(0x19);
            write_register(0x19,(data_19h+1));
        }
         
        if((data_88h&0x40)>>6){
            data_1ah=read_register(0x1a);
            write_register(0x1a,(data_1ah-1));
        }else{ 
            data_1ah=read_register(0x1a);
            write_register(0x1a,(data_1ah+1));
        }
    }
}

static void ana_dc_calib_save(char band, short index){
    
    unsigned short data_19h;
    unsigned short data_1ah;
    
    data_19h=read_register(0x19);
    data_1ah=read_register(0x1a);
    
    
    if(H_BAND==band){        
        gsm_agc_calib_ana_hband[index][0]=data_19h;
        gsm_agc_calib_ana_hband[index][1]=data_1ah;
    } 
    else if(L_BAND==band){
        gsm_agc_calib_ana_lband[index][0]=data_19h;
        gsm_agc_calib_ana_lband[index][1]=data_1ah;
    } 
}
        
static void gsm_rx_ana_dc_cal_indicator(char band, short index){

    approach();
    ana_dc_calib_save(band,index);
       
}

static void gsm_rx_dig_dc_cal_indicator(char band,int index) property(loop_free){
    short data_1f0h,data_1f1h,data_1f2h,data_1f3h;    
    short data_i,data_q;
    
    write_register(0x70,0xc0); //dc_rx_cal_en=1,dcc1_bypass=1,dcc1_mode_ct=000
    write_register(0x71,0x80); //dcc1_lpf_typ=1(IIR)
    write_register(0x72,0x7f); //filter_time
    write_register(0x73,0x88); //dcc2_bypass=1,dcc2_lpf_typ=1(IIR)
    write_register(0x74,0x08); //dcc1_bw=00001000(8*124Hz=992Hz)
    write_register(0x75,0x00); 
    
    //write_register(0x19,gsm_agc_table_analog[index][0]);//set ana calib dc,is it a must?No!
    //write_register(0x1a,gsm_agc_table_analog[index][1]);
    
    write_register(0x6a,0x00);
    write_register(0x6b,0x00);
    write_register(0x6e,0x00);
    write_register(0x6f,0x00); //set digital DC initial value
	
    //write_register(0x7b,0x80);
    delay_calib(33); //delay 1us
    write_register(0x7b,0x83);
    
    delay_calib(36000); //delay 10ms
    
    write_register(0xdf,0x87); //dc_hold_en=1
    
    write_register(0xfe,0x01);
    data_1f0h=read_register(0xf0);
    data_1f1h=read_register(0xf1);
    data_1f2h=read_register(0xf2);
    data_1f3h=read_register(0xf3);
    write_register(0xfe,0x00);
    
    data_i=((((data_1f0h<<8)|data_1f1h)&0xffff)<<2);
    data_q=((((data_1f2h<<8)|data_1f3h)&0xffff)<<2);

    write_register(0x70,0x00); //dc_rx_cal_en=0,dcc1_mode_ct=000
    write_register(0xdf,0x07); //dc_hold_en=0  

    
    if(H_BAND==band){
        gsm_agc_calib_dig_hband[index][0]=(data_i>>8)&0xff;
        gsm_agc_calib_dig_hband[index][1]=(data_i)&0xff;
        gsm_agc_calib_dig_hband[index][2]=(data_q>>8)&0xff;
        gsm_agc_calib_dig_hband[index][3]=(data_q)&0xff;        
    } 
    else if(L_BAND==band){
        gsm_agc_calib_dig_lband[index][0]=(data_i>>8)&0xff;
        gsm_agc_calib_dig_lband[index][1]=(data_i)&0xff;
        gsm_agc_calib_dig_lband[index][2]=(data_q>>8)&0xff;
        gsm_agc_calib_dig_lband[index][3]=(data_q)&0xff;
    }      
    
}


static void gsm_rx_on_hband(void){

        //write_register(0x46,(data_46h&0x0f)|0x00); //freq_former_shift set, must set in RX? gaokun
        
        //mixer,pga,vco dac set
        write_register(0x61,0x00); /*Com_mode=00,no need to set it,since it works with rx agc to control
                                     the digital gain at d6H,01 is set to edge,11 is set to wcdma, gaokun*/
        write_register(0x96,0x47); //rxclk_polarity=0,digrf_rx_resetn=0
        //write_register(0xb7,0x71);
        //change to get more blocking result, gaokun
        write_register(0x14,0x58); //lna_vbit=0100
        write_register(0x15,0x5e); //lna_bias=00
        write_register(0x17,0x00); //pga_bw_rbit=00,pga_bw_cbit=00
        write_register(0x18,0xc0); //pga mode set
        //write_register(0x1b,0x08); //filter_tuning=1000
        //write_register(0xf6,0x00); //bw_sel[3:0]=0000
        
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
        /*
        write_register(0xc2,0x37);
        write_register(0xc3,0xdf); //dac2 set
        //write_register(0xf0,0x06); 
        //write_register(0xf1,0x34); //ramp set
        
        //write_register(0xda,0x05); //iq_swap set
        */
        //power on
        write_register(0x12,0x78);          
        //write_register(0x13,0x8c); //power on,pu_dac=0,if use analog interface this bit need to set 1, gaokun
        if(rda_8850e_tcxo){
            write_register(0x13,0x8e);//vctcxo,0x8c+pu_afc
        }else{
            write_register(0x13,0x8c);//crystal  
        }      
        //pll reset
        write_register(0xbd,0xa8); //mdll_startup=1
        write_register(0x92,0x49); //Rxpll_cal_resetn=0
        write_register(0x4b,0x20); //Resetn_sdm_wd=0,tx_rx_sdm_wd=0
        
        write_register(0xbd,0x28); //mdll_startup=0
        write_register(0x92,0xc9); //Rxpll_cal_resetn=1
        write_register(0x4b,0xa0); //Resetn_sdm_wd=1,tx_rx_sdm_wd=0
        
        delay_calib(1650); //delay 50us
        
        //adc,filter,dsp reset
        /*
        write_register(0xb3,0xb4); //adc_rstn=1
        write_register(0xb3,0x94); //adc_rstn=0
        write_register(0x1c,0x80); //filter_rstn=0
        write_register(0x1c,0x90); //filter_rstn=1
        */
        write_register(0x96,0xc7); //rxclk_polarity=0,digrf_rx_resetn=1
        
        //write_register(0x7b,0x80); //Digrf_en=1,Dsp_resetn_rx=0,Dsp_resetn_tx=0
        write_register(0x7b,0x83); //Digrf_en=1,Dsp_resetn_rx=1,Dsp_resetn_tx=1
}

static void gsm_rx_on_lband(void){

        //write_register(0x46,(data_46h&0x0f)|0x00); //freq_former_shift set, must set in RX? gaokun
        
        //mixer,pga,vco dac set
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
        //write_register(0x1b,0x08); //filter_tuning=1000
        //write_register(0xf6,0x00); //bw_sel[3:0]=0000
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
        
        //write_register(0xc2,0x37);
        //write_register(0xc3,0xdf); //dac2 set
        //write_register(0xf0,0x06); 
        //write_register(0xf1,0x34); //ramp set

        //write_register(0xda,0x05); //iq_swap set
        
        //power on
        write_register(0x12,0x78);          
        //write_register(0x13,0x8c); //power on,pu_dac=0,if use analog interface this bit need to set 1, gaokun
        if(rda_8850e_tcxo){
            write_register(0x13,0x8e);//vctcxo,0x8c+pu_afc
        }else{
            write_register(0x13,0x8c);//crystal 
        }       
        //pll reset
        write_register(0xbd,0xa8); //mdll_startup=1
        write_register(0x92,0x49); //Rxpll_cal_resetn=0
        write_register(0x4b,0x20); //Resetn_sdm_wd=0,tx_rx_sdm_wd=0
        
        write_register(0xbd,0x28); //mdll_startup=0
        write_register(0x92,0xc9); //Rxpll_cal_resetn=1
        write_register(0x4b,0xa0); //Resetn_sdm_wd=1,tx_rx_sdm_wd=0
        
        delay_calib(1650); //delay 50us
        
        //adc,filter,dsp reset
        /*       
        write_register(0xb3,0xb4); //adc_rstn=1
        write_register(0xb3,0x94); //adc_rstn=0
        write_register(0x1c,0x80); //filter_rstn=0
        write_register(0x1c,0x90); //filter_rstn=1
        */
        write_register(0x96,0xc7); //rxclk_polarity=0,digrf_rx_resetn=1
        
        //write_register(0x7b,0x80); //Digrf_en=1,Dsp_resetn_rx=0,Dsp_resetn_tx=0
        write_register(0x7b,0x83); //Digrf_en=1,Dsp_resetn_rx=1,Dsp_resetn_tx=1
}



extern "C" void gsm_dc_calib_isr() property(isr loop_levels_1){

    response_bb(21,3,(0xa0));
    unsigned short index_ana;

    //wcdma_rf_initial(); //initial
    
    write_register(0x36,0x7b); //change clock 7 to 011,26M/128,it last about 5us,together with dummy_read delay set, gaokun
    
    gsm_rx_calib_freq_hband(); //freq
  
    gsm_rx_on_hband(); //gsm_rx_on
   
    for(index_ana=0;index_ana<12;index_ana++)
    {               
        write_register(0x3c,0x80);//filter_cal_clk
        write_register(0x1c,0xf0);//filter_cal_enable
                
        write_register(0x19,0x80);
        write_register(0x1a,0x80);//start from middle
        
        gsm_rx_calib_ana_gain(H_BAND,index_ana);
                
        gsm_rx_ana_dc_cal_indicator(H_BAND,index_ana);
        
        write_register(0x3c,0x00);//filter_cal_clk
        write_register(0x1c,0x90);//filter_cal_enable
        
        gsm_rx_dig_dc_cal_indicator(H_BAND,index_ana);

    }
   
    gsm_rx_tx_off(); //gsm_rx_off
    
    gsm_rx_calib_freq_lband(); //freq
    
    gsm_rx_on_lband(); //gsm_rx_on
    
          
    for(index_ana=0;index_ana<12;index_ana++){
                  
        write_register(0x3c,0x80);//filter_cal_clk
        write_register(0x1c,0xf0);//filter_cal_enable
                
        write_register(0x19,0x80);
        write_register(0x1a,0x80);//start from middle
        
        gsm_rx_calib_ana_gain(L_BAND,index_ana);
                
        gsm_rx_ana_dc_cal_indicator(L_BAND,index_ana);
        
        write_register(0x3c,0x00);//filter_cal_clk
        write_register(0x1c,0x90);//filter_cal_enable
        
        gsm_rx_dig_dc_cal_indicator(L_BAND,index_ana);
               
    }
       
    gsm_rx_tx_off(); //gsm_rx_off
    
    thermometer_cycle();
    
    clear_interrupt(0x01,0x20);
  
    response_bb(21,3,(0x40));
}

         
