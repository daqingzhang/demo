#include <register.h>
#include <calib_wcdma.h>
#include <auxiliary.h>

extern void gsm_wcdma_rf_initial(void);
extern void wcdma_rx_tx_on(void);
extern void wcdma_rx_tx_off(void);
extern unsigned char wcdma_agc_hb_table_analog[CALIB_WCDMA_RX_ANA_GAIN_NUM][3];
extern unsigned char wcdma_agc_hb_table_analog_ln41[CALIB_WCDMA_RX_ANA_GAIN_NUM][3];
extern unsigned short wcdma_apc_hb_table_ana_gain_0[CALIB_WCDMA_TX_ANA_GAIN_NUM];
extern unsigned short wcdma_apc_hb_table_ana_gain_1[CALIB_WCDMA_TX_ANA_GAIN_NUM];
extern unsigned short* wcdma_apc_hb_table_digital_gain_pt[CALIB_WCDMA_TX_ANA_GAIN_NUM];
extern unsigned short* wcdma_apc_lb_table_digital_gain_pt[CALIB_WCDMA_TX_ANA_GAIN_NUM];
extern unsigned short wcdma_apc_hb_table_length[CALIB_WCDMA_TX_ANA_GAIN_NUM];

extern short rda_8850e_logic_version;
extern short rda_8850e_tcxo;

char wcdma_agc_calib_ana_hb[CALIB_WCDMA_RX_ANA_GAIN_NUM][2] = {{33,11},{33,11},{33,11},{33,11},{33,11},{33,11},{33,11}};
char wcdma_agc_calib_dig_hb[CALIB_WCDMA_RX_ANA_GAIN_NUM][4] = {{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{33,0,0,0},{33,0,0,0},{33,0,0,0}};
char wcdma_agc_calib_ana_lb[CALIB_WCDMA_RX_ANA_GAIN_NUM][2] = {{33,11},{33,11},{33,11},{33,11},{33,11},{33,11},{33,11}};
char wcdma_agc_calib_dig_lb[CALIB_WCDMA_RX_ANA_GAIN_NUM][4] = {{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0},{33,0,0,0},{33,0,0,0},{33,0,0,0}};

short wcdma_apc_calib_ana_hb[CALIB_WCDMA_TX_ANA_GAIN_NUM][2] = {{22,55},{22,55},{22,55},{22,55},{22,55},{22,55},{22,55},{22,55},{22,55},{22,55},{22,55}};
short wcdma_apc_calib_ana_lb[CALIB_WCDMA_TX_ANA_GAIN_NUM][2] = {{22,55},{22,55},{22,55},{22,55},{22,55},{22,55},{22,55},{22,55},{22,55},{22,55},{22,55}};


void wcdma_calib_rx_tx_on(short band_sel)
{
    
    write_register(0x22,0xcb);//enlarge pll bandwidth to avoid freq shift
    write_register(0x70,0x40);

    write_register(0x12,0xff);
    if(rda_8850e_tcxo){
        write_register(0x13,0x6e);//vctcxo,0x6c+pu_afc
    }else{
        write_register(0x13,0x6c);//crystal
    }
    write_register(0x17,0x94);//pga_bw_rbit=0b10  
    
    write_register(0x1a,0x80);
    write_register(0x1b,0x00);//set to default after gsm,1123
    write_register(0x29,0x88);
    write_register(0xd2,0x47);//6db
    write_register(0xf6,0x18);//1123
    
    write_register(0xbc,0x7c);
    write_register(0x2f,0xc8);
    if(band_sel)
    {
        //item:8850E_RF_ON_H

        write_register(0x14,0x5f);
        write_register(0x15,0x14);//54,0609
        write_register(0x18,0x00);//00--->30,min range of dc 0706
        write_register(0x19,0x80);        
        write_register(0x1c,0x00);
        write_register(0x1d,0x88);//wcdma:filter_mode_gsm=0,filter_mode_td=0
        write_register(0x1c,0x10);
        //write_register(0x1d,0x88);//filter_rstn;  gsm:wcdma:filter_mode_gsm=1,filter_mode_td=X
        write_register(0x60,0x00);        
        write_register(0x61,0xc0);//wcdma COM_MODE=11  GSM COM_MODE=10
        //write_register(0x64,0x13); //0x12 dac test mode;0x18 bypass tx digital gain;0612 
        write_register(0x64,0x00);                     
        write_register(0x1e,0x02);
        write_register(0x1f,0x94);   
        write_register(0x2c,0x68);
        write_register(0x2d,0x45);
        write_register(0x2c,0xe8);
        //write_register(0x2d,0x45);//tmx_op_rstn      
        write_register(0xb4,0x04);
        write_register(0xb5,0x01);
        //write_register(0xb4,0x04);
        write_register(0xb5,0x05);//dac_rstn                
        
        write_register(0xfe,0x01);//page=01
        write_register(0x10,0x0f);
        write_register(0x11,0xe8);
        
        write_register(0x13,0x08);//notch_en  //// ------------- always close notch, at 0827
        
        write_register(0x18,0x10);
        write_register(0x19,0x18);
        write_register(0xfe,0x00); //page=00 

        
        write_register(0x7a,0x02);   
        write_register(0x7b,0x00);
        //write_register(0x4c,0x07);
        write_register(0x4c,0x0e); 
        delay_calib(33); 
        write_register(0x7b,0x03); //Dsp_resetn_rx/tx  
        //write_register(0x4c,0x87);  //adc_pll reset 
        write_register(0x4c,0x8e);
        
        if(3==rda_8850e_logic_version){
            write_register(0xe8,0x0d);//dev_3.0
            write_register(0x11,0x40); 
        }else if(4==rda_8850e_logic_version){
            write_register(0xe8,0x2f);//8850ew420_mipi
            write_register(0x11,0x00);
            mipi_rffe_register_write(3,0,0x88);
        }else if(5==rda_8850e_logic_version){
            write_register(0xe8,0x07);//8850ew420_mipi_v2000_6190_v1
            write_register(0x11,0x00);
            mipi_rffe_register_write(3,0,0x88);            
        }else{
            write_register(0xe8,0x8c);//0c-->8c,0616 pa_on+lna
            write_register(0x11,0x80); 
        } 
             
       
    }else{
        //item:8850E_RF_ON_L
 
        write_register(0x14,0xaf);
        write_register(0x15,0x1e);
        write_register(0x18,0x00);//00--->30,min range of dc 0706
        write_register(0x19,0x80);        
        write_register(0x1c,0x00);
        write_register(0x1d,0x88);//wcdma:filter_mode_gsm=0,filter_mode_td=0
        write_register(0x1c,0x10);
        //write_register(0x1d,0x88);//filter_rstn;  gsm:wcdma:filter_mode_gsm=1,filter_mode_td=X        
        write_register(0x60,0x00);        
        write_register(0x61,0xc0);//wcdma COM_MODE=11  GSM COM_MODE=10
        //write_register(0x64,0x13); //0x12 dac test mode;0x18 bypass tx digital gain;0612
        write_register(0x64,0x00);
        write_register(0x1e,0x02);
        write_register(0x1f,0x08);   
        write_register(0x2c,0x68);
        write_register(0x2d,0x45);
        write_register(0x2c,0xe8);
        //write_register(0x2d,0x45);//tmx_op_rstn       
        write_register(0xb4,0x04);
        write_register(0xb5,0x01);
        //write_register(0xb4,0x04);
        write_register(0xb5,0x05);//dac_rstn 
                        
        write_register(0xfe,0x01);//page=01
        write_register(0x10,0x0f);
        write_register(0x11,0xe8); 
        
        write_register(0x13,0x08);//notch_en  //// ------------- always close notch, at 0827  
        write_register(0x18,0x10);
        write_register(0x19,0x04);
        write_register(0xfe,0x00); //page=00

        write_register(0x7a,0x02);   
        write_register(0x7b,0x00);
        write_register(0x4c,0x0e); 
        delay_calib(33); 
        write_register(0x7b,0x03); //Dsp_resetn_rx/tx
        write_register(0x4c,0x8e);  
        
        if(3==rda_8850e_logic_version){
            write_register(0xe8,0x45);//dev_3.0
            write_register(0x11,0x00);
        }else if(4==rda_8850e_logic_version){
            write_register(0xe8,0x0f);//8850ew420_mipi
            write_register(0x11,0x00);
            mipi_rffe_register_write(3,0,0x22);
        }else if(5==rda_8850e_logic_version){
            write_register(0xe8,0x0f);//8850ew420_mipi_v2000_6190_v1
            write_register(0x11,0x00);
            mipi_rffe_register_write(3,0,0x22);            
        }else{
            write_register(0xe8,0x45);//0c-->8c,0616 pa_on
            write_register(0x11,0x40);    
        } 
    }   
    
            
} 

static void wcdma_rx_calib_freq(char band){
    
    int rxpll_target=0;
    int rxpll_target_median=0;
    int mdll_div_num;
    int data_50h_freq;
    int data_bch=0;//mdll_div_num
    int data_4bh=0;//mdll_div_num
    int data_46h=0;

    data_bch = read_register(0xbc);
    data_4bh = read_register(0x4b);
    data_46h = read_register(0x46);        
    
    if(H_BAND==band)
        data_50h_freq=(2160)&0xffff;//2112
    else if(L_BAND==band)
        data_50h_freq=(880*2)&0xffff;
    //mdll_div_num
    mdll_div_num = 7;        
    write_register(0xbc,(data_bch&0x0f)|0x70);
    write_register(0x4b,(data_4bh&0xfc)|0x00);

    //freq calculation        
    rxpll_target_median = data_50h_freq*1290555;
    rxpll_target = (rxpll_target_median>>17)&0x7fff;
    if((rxpll_target_median)&0x00010000)
        rxpll_target += 1;
        
    
    int rx_freq_msb,rx_freq_lsb,rx_freq_remainder;
    rx_freq_msb = ((data_50h_freq<<8)/(mdll_div_num*26))&0xfff;
    rx_freq_remainder = (data_50h_freq<<8)%(mdll_div_num*26);    
    rx_freq_lsb = ((rx_freq_remainder<<16)/(mdll_div_num*26))&0xffff;
    
    int data01,data02,data03,data04,data05,data06;
    data01=(rxpll_target>>8)&0xff;
    data02=(rxpll_target)&0xff;
    data03=(((data_46h&0xf0)|((rx_freq_msb>>8)&0x0f))&0xff);
    data04=(rx_freq_msb)&0xff;
    data05=(rx_freq_lsb>>8)&0xff;
    data06=(rx_freq_lsb)&0xff;
    
    write_register(0x90,data01);
    write_register(0x91,data02);// write in rxpll_target        

    write_register(0x46,data03);
    write_register(0x47,data04); 
    write_register(0x48,data05);
    write_register(0x49,data06);// write in rx_freq  
           
    // Reset    
    write_register(0x92,0x49);
    write_register(0x4b,((mdll_div_num-7)&0x03)|0x20);
    write_register(0xbd,0xa8);
    
    delay_calib(33);
    
    write_register(0x92,0xc9);//RXPLL_cal_resetn
    write_register(0x4b,((mdll_div_num-7)&0x03)|0xa0);//sdm_resetn
    write_register(0xbd,0x28);//mdll_startup

} 

static void wcdma_tx_calib_freq(char band){
    
    int data_bch=0;//mdll_div_num
    int data_41h=0;//mdll_div_num
    int data_42h=0;
    int txpll_target_msb=0;
    int txpll_target_remainder=0;
    int mdll_div_num;
    int data_54h_freq;
    
  
    data_bch = read_register(0xbc);
    data_41h = read_register(0x41);
    data_42h = read_register(0x42);
 
    if(H_BAND==band)
        data_54h_freq=(1950*20)&0xffff;//2112
    else if(L_BAND==band)
        data_54h_freq=(835*40)&0xffff;  

    mdll_div_num = 7;        
    write_register(0xbc,(data_bch&0x0f)|0x70);
    write_register(0x41,(data_41h&0xfc)|0x00);
    //freq calculation
    
    
    //txpll_target = ((data_54h_freq<<8)/26)&0xffff;
    //tx_freq = (((data_54h_freq<<16)/(mdll_div_num*26))<<8)&0xfffffff;
    
    txpll_target_msb = ((data_54h_freq<<8)/(26*20))&0xffff;
    txpll_target_remainder = ((data_54h_freq<<8)%(26*20))&0xffff;
    if(txpll_target_remainder>((26*20)/2))
        txpll_target_msb += 1;    
        
    
    int tx_freq_msb,tx_freq_lsb,tx_freq_remainder;
    tx_freq_msb = ((data_54h_freq<<8)/(mdll_div_num*26*20))&0xffff;
    tx_freq_remainder = (data_54h_freq<<8)%(mdll_div_num*26*20);    
    tx_freq_lsb = ((tx_freq_remainder<<16)/(mdll_div_num*26*20))&0xffff;
    
    int data01,data02,data03,data04,data05,data06;
    data01=(txpll_target_msb>>8)&0xff;
    data02=(txpll_target_msb)&0xff;
    data03=(((data_42h&0xf0)|((tx_freq_msb>>8)&0x0f))&0xff);
    data04=(tx_freq_msb)&0xff;
    data05=(tx_freq_lsb>>8)&0xff;
    data06=(tx_freq_lsb)&0xff; 
   

    write_register(0x3a,data01);
    write_register(0x3b,data02);        

    write_register(0x42,data03);
    write_register(0x43,data04); 
    write_register(0x44,data05);
    write_register(0x45,data06);   
            
    // Reset        
    write_register(0x39,0x00);
    write_register(0x41,((mdll_div_num-7)&0x03)|0x20);
    write_register(0xbd,0xa8);

    delay(156);
    
    write_register(0x39,0x01);//TXPLL_cal_resetn    
    write_register(0x41,((mdll_div_num-7)&0x03)|0xa0);//sdm_resetn  
    write_register(0xbd,0x28);//mdll_startup

} 


static void wcdma_rx_calib_ana_gain(char band,short index){
    
    short data_14h;
    unsigned char (*analog_p)[3];
    data_14h=read_register(0x14);
        
    if(H_BAND==band){     
        
        //write_register(0xe8,0x00);//07
        //write_register(0x11,0x00);//40,set front_end to band5 to avoid noise
        if((4!=rda_8850e_logic_version)||(5!=rda_8850e_logic_version)){
            //write_register(0xe8,0x06);
            write_register(0x11,0xc0);// close antenna
        }

        if(3==rda_8850e_logic_version){
            if(index<CALIB_WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x0d);//H gain
            }else{
                write_register(0xe8,0x0f);//M gain
            } 
        }else if(4==rda_8850e_logic_version){
            if(index<CALIB_WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x0f);//0x2f);//H gain
            }else{
                write_register(0xe8,0x0d);//0x2d);//M gain
            } 
        }else if(5==rda_8850e_logic_version){
            if(index<CALIB_WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x0f);//0x07);//H gain,8850ew420_mipi_v2000_6190_v1
            }else{
                write_register(0xe8,0x0d);//0x05);//M gain,8850ew420_mipi_v2000_6190_v1
            }              
        }else{      
            if(index<CALIB_WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x0c);//H gain
            }else{
                write_register(0xe8,0x0e);//M gain
            } 
        }         

        
        analog_p = wcdma_agc_hb_table_analog;
        if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
            analog_p = wcdma_agc_hb_table_analog_ln41;
        }
        write_register(0x14,((data_14h&0xf0)|((*(analog_p+index))[2]))); 
        write_register(0x16,((*(analog_p+index))[0]));
        write_register(0x10,((*(analog_p+index))[1])); 
        
    }else if(L_BAND==band){
                                    
        //write_register(0xe8,0x00);//0e
        //write_register(0x11,0x00);//80,set front_end to band1 to avoid noise 
        if((4!=rda_8850e_logic_version)||(5!=rda_8850e_logic_version)){
            write_register(0xe8,0x06);///////////////////////////////////////////////need take care!!!0714
            write_register(0x11,0xc0);  
        }      
               
        if(3==rda_8850e_logic_version){
            if(index<CALIB_WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x05);//H gain
            }else{
                write_register(0xe8,0x07);//M gain
            }  
        }else if(4==rda_8850e_logic_version){
            if(index<CALIB_WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x2f);//0x0f);//H gain
            }else{
                write_register(0xe8,0x2d);//0x0d);//M gain
            } 
        }else if(5==rda_8850e_logic_version){
            if(index<CALIB_WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x07);//0x0f);//H gain,8850ew420_mipi_v2000_6190_v1
            }else{
                write_register(0xe8,0x05);//0x0d);//M gain,8850ew420_mipi_v2000_6190_v1
            }              
        }else{
            if(index<CALIB_WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x05);//H gain
            }else{
                write_register(0xe8,0x07);//M gain
            } 
        } 
                        
        analog_p = wcdma_agc_hb_table_analog;
        if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
            analog_p = wcdma_agc_hb_table_analog_ln41;
        }
        write_register(0x14,((data_14h&0xf0)|((*(analog_p+index))[2]))); 
        write_register(0x16,((*(analog_p+index))[0]));
        write_register(0x10,((*(analog_p+index))[1])); 
    }        
}

static void wcdma_tx_calib_ana_gain(char band,short index)
{   
    
    unsigned short* dig_end;
    unsigned short* dig_pt;
    unsigned short index_ana=0;
    unsigned short data_ana_0,data_ana_1,data_dig,dig_len;
    

    index_ana = index;
    //write_register(0xcc,index_ana+1);
    data_ana_0 = wcdma_apc_hb_table_ana_gain_0[index_ana];
    data_ana_1 = wcdma_apc_hb_table_ana_gain_1[index_ana];
   
    if(H_BAND==band)
    {
        //item:8850E_RF_ON_H
        dig_pt = wcdma_apc_hb_table_digital_gain_pt[index_ana];
        dig_len = wcdma_apc_hb_table_length[index_ana];
        data_dig=*(dig_pt+dig_len+CALIB_WCDMA_TX_DC_DIG_GAIN_OFFSET);
           
    }else{  
        //item:8850E_RF_ON_L
        dig_pt = wcdma_apc_lb_table_digital_gain_pt[index_ana];
        dig_len = wcdma_apc_hb_table_length[index_ana];
        data_dig=*(dig_pt+dig_len+CALIB_WCDMA_TX_DC_DIG_GAIN_OFFSET);
 
    } 
    
    write_register(0x2e,data_ana_0);
    write_register(0x2a,data_ana_1);
    write_register(0x66,((data_dig>>8)&0xe0));
    write_register(0x67,(data_dig&0xff));        
}   


static int dummy_read(int addr){
    int data=0;
    //delay_calib(165);//5us,freq_clk_div_7=26m/128
    //delay_calib(35);//0.3us,freq_clk_div_7=26m/8,at least 1us because of filter
    delay_calib(50);//50-1.5us,50--83-2.5us
    data=read_register(addr);
    return(data);
}

static int arithmetic_calib(int value,char operation){
    int target=value;
    if(ADD==operation){    
        if(target>=0xff){
            target=0xff;
        }else{
            target=value+1; 
        } 
    }
    if(SUB==operation){      
        if(target<=0x00){
            target=0x00;
        }else{
            target=value-1;
        }             
    }
    return(target);
}
    

static void ana_dc_calib_save(char band, short index, char path){
    if(H_BAND==band){
        if(IPATH==path){
            wcdma_agc_calib_ana_hb[index][0]=read_register(0x19);
        }else{
            wcdma_agc_calib_ana_hb[index][1]=read_register(0x1a);
        }
    } 
    else if(L_BAND==band){
        if(IPATH==path){
            wcdma_agc_calib_ana_lb[index][0]=read_register(0x19);
        }else{
            wcdma_agc_calib_ana_lb[index][1]=read_register(0x1a);
        }
    } 
}      

static void approach_max(char band, short index)
{
    int loop_num=0;
    volatile int data_88h;
    volatile int data_19h;
    volatile int data_1ah;
    //int data_ceh,data_cfh;
    //data_ceh=read_register(0xce);
    //data_cfh=read_register(0xcf);
   // data_cfh=((data_ceh<<8)|(data_cfh&0xff))&0xffff;
   // for(;loop_num<data_cfh;loop_num++){
    for(;loop_num<0xfff;loop_num++){
        data_88h=dummy_read(0x88);
        if((data_88h&0x80)>>7){
            data_19h=read_register(0x19);
            data_19h=arithmetic_calib(data_19h,ADD);
            write_register(0x19,data_19h);
        }else{        
            data_19h=read_register(0x19);
            data_19h=arithmetic_calib(data_19h,SUB);
            write_register(0x19,data_19h);
        } 
        if((data_88h&0x40)>>6){
            data_1ah=read_register(0x1a);
            data_1ah=arithmetic_calib(data_1ah,ADD);
            write_register(0x1a,data_1ah);
        }else{ 
            data_1ah=read_register(0x1a);
            data_1ah=arithmetic_calib(data_1ah,SUB);
            write_register(0x1a,data_1ah);
        }
    }
/*    
    int data01,data02;
    data01=read_register(0x19);
    data02=read_register(0x1a);
    write_register(0xfe,0x01);
    write_register((0xe0+index),data01);
    write_register((0xe8+index),data02);
    write_register(0xfe,0x00);    
*/    
    ana_dc_calib_save(band,index,IPATH);
    ana_dc_calib_save(band,index,QPATH);
}
    

 
static void wcdma_rx_dig_dc_cal_indicator(char band, int index)
{

    unsigned int data_1f0h,data_1f1h,data_1f2h,data_1f3h;    
    unsigned int data_i,data_q;
    
    write_register(0x70,0xc0);//cal_en,dcc_mode=011,db---c3,83---80
    //write_register(0x72,0x7f);//filter_time
    write_register(0x72,0x3f);     
    //write_register(0x74,0x0f);//BW1 is used
    write_register(0x74,0x01);
    //write_register(0x75,0x0f);
    
    if(H_BAND==band){
        write_register(0x19,wcdma_agc_calib_ana_hb[index][0]);//set ana calib gain
        write_register(0x1a,wcdma_agc_calib_ana_hb[index][1]);
    } 
    else if(L_BAND==band){
        write_register(0x19,wcdma_agc_calib_ana_lb[index][0]);//set ana calib gain
        write_register(0x1a,wcdma_agc_calib_ana_lb[index][1]);
    } 
    
    write_register(0x6a,0x00);//default dig calib gain
    write_register(0x6b,0x00);
    write_register(0x6e,0x00);
    write_register(0x6f,0x00); 
    
    write_register(0x7b,0x00);
    //delay(156);
    delay_calib(33);//1us
    write_register(0x7b,0x03);
/*    
    int data1a2,data1a3,data1a4;
    write_register(0xfe,0x01);
    data1a2=read_register(0xa2);
    data1a3=read_register(0xa3);
    data1a4=read_register(0xa4);
    write_register(0xfe,0x00);
    data1a2=(((data1a2<<16)&0xff0000)|((data1a3<<8)&0xff00)|(data1a4&0xff))&0xffffff;
*/
    delay_calib(33000*3);//150us-->10ms
/*    
    unsigned int i,total_i=0,total_q=0;
    
    for(i=0;i<16;i++){
        
        write_register(0xfe,0x01);
        data_1f0h=read_register(0xf0);
        data_1f1h=read_register(0xf1);
        data_1f2h=read_register(0xf2);
        data_1f3h=read_register(0xf3);
        write_register(0xfe,0x00);
        
        data_1f0h=(((data_1f0h<<8)|(data_1f1h&0xff))&0x3fff);
        data_1f2h=(((data_1f2h<<8)|(data_1f3h&0xff))&0x3fff);
        total_i+=data_1f0h;
        total_q+=data_1f2h;

    }

    data_i=((total_i/16)&0xffff)<<2;
    data_q=((total_q/16)&0xffff)<<2;
*/
    write_register(0xfe,0x01);
    data_1f0h=read_register(0xf0);
    data_1f1h=read_register(0xf1);
    data_1f2h=read_register(0xf2);
    data_1f3h=read_register(0xf3);
    write_register(0xfe,0x00);
         
    data_i=(((data_1f0h<<8)|(data_1f1h&0xff))&0xffff)<<2;
    data_q=(((data_1f2h<<8)|(data_1f3h&0xff))&0xffff)<<2;

    
    if(H_BAND==band){
        wcdma_agc_calib_dig_hb[index][0]=(data_i>>8)&0xff;
        wcdma_agc_calib_dig_hb[index][1]=(data_i)&0xff;
        wcdma_agc_calib_dig_hb[index][2]=(data_q>>8)&0xff;
        wcdma_agc_calib_dig_hb[index][3]=(data_q)&0xff;
    } 
    else if(L_BAND==band){
        wcdma_agc_calib_dig_lb[index][0]=(data_i>>8)&0xff;
        wcdma_agc_calib_dig_lb[index][1]=(data_i)&0xff;
        wcdma_agc_calib_dig_lb[index][2]=(data_q>>8)&0xff;
        wcdma_agc_calib_dig_lb[index][3]=(data_q)&0xff;
    } 
/*    
    write_register(0xfe,0x01);
    write_register((0xe0+index),data_1f0h);
    write_register((0xe8+index),data_1f1h);
    write_register(0xfe,0x00);                
*/    
} 

static void wcdma_tx_dac_write(short path,short value)
{
    short v_msb,v_lsb;
    
    v_msb=(value&0xf00)>>8;
    v_lsb=value&0xff;
    
    if(IPATH==path){
        write_register(0xf4,(0x80|v_msb));
        write_register(0xf5,v_lsb);
    }else if(QPATH==path){
        write_register(0xfe,0x02);
        write_register(0x2c,v_msb);
        write_register(0x2b,v_lsb);
        write_register(0xfe,0x00);     
    }
}

static void ana_tx_dc_calib_save(char band, short index, char path, short calib_value){
    if(H_BAND==band){
        if(IPATH==path){
            wcdma_apc_calib_ana_hb[index][0]=calib_value;
        }else{
            wcdma_apc_calib_ana_hb[index][1]=calib_value;
        }
    } 
    else if(L_BAND==band){
        if(IPATH==path){
            wcdma_apc_calib_ana_lb[index][0]=calib_value;
        }else{
            wcdma_apc_calib_ana_lb[index][1]=calib_value;
        }
    } 
} 

static short accumulate_88h_half(short total_num,char path)
{
    short num;
    volatile short data_88h=0;
    volatile short accu_88h=0;
    if(QPATH==path){// due to 0x220 iq swap, change to qpath
        for(num=0;num<total_num;num++){// I
            data_88h=dummy_read(0x88);
            if(data_88h&0x04){//i,1-,0+
                accu_88h+=1;
            }
        }
    }else{
        for(num=0;num<total_num;num++){// Q
            data_88h=dummy_read(0x88);
            if(data_88h&0x02){//i,1-,0+
                accu_88h+=1;
            }
        } 
    }       
    return (accu_88h);
}


static void tx_dc_adjust_half(char band, short index)
{
    short count_num;
    volatile short i_dac=0x800;
    volatile short q_dac=0x800;
    volatile short accu_88h; 
    
    for(count_num=10;count_num>=0;count_num--){
        accu_88h=accumulate_88h_half(10,IPATH);
        //write_register(0xce,accu_88h+1);// trace debug
        if(accu_88h>7){//i,1-,0+
            i_dac=i_dac-(1<<count_num);
            wcdma_tx_dac_write(IPATH,i_dac);
        }else if(accu_88h<3){        
            i_dac=i_dac+(1<<count_num);
            wcdma_tx_dac_write(IPATH,i_dac);         
        }else{
            break;
        }
    } 
    for(count_num=10;count_num>=0;count_num--){
        accu_88h=accumulate_88h_half(10,QPATH);
        //write_register(0xcf,accu_88h+1);// trace debug
        if(accu_88h>7){//i,1-,0+
            q_dac=q_dac-(1<<count_num);
            wcdma_tx_dac_write(QPATH,q_dac);
        }else if(accu_88h<3){        
            q_dac=q_dac+(1<<count_num);
            wcdma_tx_dac_write(QPATH,q_dac);         
        }else{
            break;
        }
    } 
    ana_tx_dc_calib_save(band,index,IPATH,i_dac);
    ana_tx_dc_calib_save(band,index,QPATH,q_dac);
}

static void tx_dc_approach(char band, short index)// for test only, but failed
{
    int loop_num=0;
    short i_dac=0x800;
    short q_dac=0x800;
    short data_88h;
    
    for(loop_num=0;loop_num<0x3fff;loop_num++){
        data_88h=dummy_read(0x88);
        if(data_88h&0x04){//i,1-,0+
            if(--i_dac==0)
               i_dac=0; 
            wcdma_tx_dac_write(IPATH,i_dac);
        }else{        
            if(++i_dac>=0xfff)
               i_dac=0xfff; 
            wcdma_tx_dac_write(IPATH,i_dac);            
        }
    }
    for(loop_num=0;loop_num<0x3fff;loop_num++){
        data_88h=dummy_read(0x88);     
        if(data_88h&0x02){//q
            if(--q_dac==0)
               q_dac=0;  
            wcdma_tx_dac_write(QPATH,q_dac);
        }else{ 
            if(++q_dac>=0xfff)
               q_dac=0xfff; 
            wcdma_tx_dac_write(QPATH,q_dac);            
        }
    }

    ana_tx_dc_calib_save(band,index,IPATH,i_dac);
    ana_tx_dc_calib_save(band,index,QPATH,q_dac);
}


static void wcdma_rx_calib(void)
{
    
    short index_ana;
    
    //HB
    //software_interrupt(7);//initial
    gsm_wcdma_rf_initial();
    
    write_register(0x36,0xbb);//change clock 7 to 011, 26M/128,7b--fb, change to 26m/8,fb-->bb 26m/32,bb-9b 26m/64

    //software_interrupt(6);//rf_on, need change to rx_on, changed on 0602
    wcdma_calib_rx_tx_on(H_BAND);

    wcdma_rx_calib_freq(H_BAND);//freq   
    delay_calib(4000);
    for(index_ana=0;index_ana<CALIB_WCDMA_RX_ANA_GAIN_NUM;index_ana++)
    {               
        write_register(0x3c,0x80);//filter_cal_clk
        write_register(0x1c,0x30);//filter_cal_enable        
        write_register(0x19,0x80);
        write_register(0x1a,0x80);//start from middle
        wcdma_rx_calib_ana_gain(H_BAND,index_ana);        
        approach_max(H_BAND,index_ana);
        write_register(0x3c,0x00);//filter_cal_clk
        write_register(0x1c,0x10);//filter_cal_enable
                
        wcdma_rx_dig_dc_cal_indicator(H_BAND,index_ana);
        write_register(0x70,0x40);//cal_en,bypass
        //write_register(0x72,0x05);//filter_time     
        //write_register(0x74,0xfe);//BW1 is used
        //write_register(0x75,0xc3);
        
    }
      
    // Low Band Calib 5--8
    wcdma_calib_rx_tx_on(L_BAND);
    
    wcdma_rx_calib_freq(L_BAND);//freq  
    delay_calib(4000); 
    for(index_ana=0;index_ana<CALIB_WCDMA_RX_ANA_GAIN_NUM;index_ana++)
    {               
        write_register(0x3c,0x80);//filter_cal_clk
        write_register(0x1c,0x30);//filter_cal_enable        
        write_register(0x19,0x80);
        write_register(0x1a,0x80);//start from middle
        wcdma_rx_calib_ana_gain(L_BAND,index_ana);        
        approach_max(L_BAND,index_ana);
        write_register(0x3c,0x00);//filter_cal_clk
        write_register(0x1c,0x10);//filter_cal_enable
                
        wcdma_rx_dig_dc_cal_indicator(L_BAND,index_ana);
        write_register(0x70,0x40);//cal_en,bypass
        //write_register(0x72,0x05);//filter_time     
        //write_register(0x74,0xfe);//BW1 is used
        //write_register(0x75,0xc3);
        
    }
  
    //software_interrupt(5);//rf_off 
    wcdma_rx_tx_off();
}   

static void wcdma_tx_calib(void)
{
    
    short index_ana=0;
    
    gsm_wcdma_rf_initial();

    wcdma_calib_rx_tx_on(H_BAND);
    
    wcdma_tx_calib_freq(H_BAND);//freq 
    delay_calib(4000);
    write_register(0xf4,0x80);//tx_dac_cal_en,need reset at end!
    
    write_register(0xfe,0x02);
    write_register(0x20,0x40);//tx_dac_sign,0
    write_register(0xfe,0x00);   
    
    write_register(0x64,0x00);//dac1_ts_mode
    
    write_register(0x3c,0x40);//tmx_cal_clk
    write_register(0x36,0xb7);//freq_clk_div_6,101
     
    write_register(0x29,0x89);//tmx_cal_en
          
    wcdma_tx_dac_write(IPATH,0x800);
    wcdma_tx_dac_write(QPATH,0x800);
    
    wcdma_tx_calib_ana_gain(H_BAND,index_ana);
    //tx_dc_approach(H_BAND,index_ana);
    tx_dc_adjust_half(H_BAND,index_ana);

    // L BAND 
    wcdma_calib_rx_tx_on(L_BAND);
    
    wcdma_tx_calib_freq(L_BAND);//freq 
    delay_calib(4000);
    write_register(0xf4,0x80);//tx_dac_cal_en,need reset at end!
    
    write_register(0x3c,0x40);//tmx_cal_clk
    write_register(0x36,0xb7);//freq_clk_div_6,101
     
    write_register(0x29,0x89);//tmx_cal_en
          
    wcdma_tx_dac_write(IPATH,0x800);
    wcdma_tx_dac_write(QPATH,0x800);
    
    wcdma_tx_calib_ana_gain(L_BAND,index_ana);
    //tx_dc_approach(H_BAND,index_ana);
    tx_dc_adjust_half(L_BAND,index_ana);   
    
    write_register(0xf4,0x00); 
    wcdma_rx_tx_off();
}


void irat_dc_calib_isr()
{
   
    response_bb(20,3,(0xa0));
    clear_interrupt(0x01,0x10);
    
    wcdma_rx_calib();

    wcdma_tx_calib();

    response_bb(20,3,(0x40));
 
} 

// 3         
void wcdma_rx_calib_isr()
{
    
    response_bb(2,5,(0xa0));
    clear_interrupt(0x05,0x04);
    
    wcdma_rx_calib();
    
    wcdma_tx_calib();
    
    response_bb(2,5,(0x40));

}

void wcdma_tx_calib_isr()
{
    
    response_bb(1,5,(0xa0));
    clear_interrupt(0x05,0x02);
    
    wcdma_tx_calib();
    
    response_bb(1,5,(0x40));

}
