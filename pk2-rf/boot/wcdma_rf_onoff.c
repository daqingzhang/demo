#include <rf/register.h>
#include <rf/wcdma_table.h>
#include <rf/calib_wcdma.h>
#include <rf/auxiliary.h>

/* W extern function declaration  */
extern unsigned short wcdma_tx_comp(unsigned short base, short offset);

/* W variables declaration  */
static char (*wcdma_agc_calib_ana)[2];
static char (*wcdma_agc_calib_dig)[4];

#define H_BAND 1
#define L_BAND 0

short rda_8850e_logic_version=0,rda_8850e_tcxo=0;
short logic_switch_flag=0,tcxo_read_flag=0;
short narrow_bw_search_flag=0;
short band_select_flag=H_BAND;
short last_band_select_flag=H_BAND;
short last_rf_state=0;

/* W function definition */
void wcdma_rx_tx_on(void)
{
    
    short data_12h,data_7bh;
    
    data_12h=read_register(0x12);
    data_7bh=read_register(0x7b);
    
    write_register(0x22,0xcb);//enlarge pll bandwidth to avoid freq shift
    write_register(0x70,0x40);

    write_register(0x12,((data_12h&0x80)|0x78));//7f
    if(rda_8850e_tcxo){
        write_register(0x13,0x0e);//vctcxo,0x6c+pu_afc //6e
    }else{
        write_register(0x13,0x0c);//crystal //6c
    }
    write_register(0x17,0x94);//pga_bw_rbit=0b10  
    
    write_register(0x1a,0x80);
    write_register(0x1b,0x00);//set to default after gsm,1123
    write_register(0x29,0x88);
    write_register(0xd2,0x47);//6db
    write_register(0xf6,0x18);//1123
    
    write_register(0xbc,0x7c);
    write_register(0x2f,0xc8);

    // lock at 10700 at very first power on, but there is a difference when sleep-->wakeup
    if(H_BAND==last_band_select_flag)
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
        //write_register(0x10,0x0f);
        //write_register(0x11,0xe8);
        write_register(0x10,0x07);// backup switch of pll modem interface
        write_register(0x11,0x68);// backup switch of spll wd
        
        write_register(0x13,0x08);//notch_en  //// ------------- always close notch, at 0827
        
        write_register(0x18,0x10);
        write_register(0x19,0x18);
        write_register(0xfe,0x00); //page=00 

        
        write_register(0x7a,0x02);   
        write_register(0x7b,((data_7bh&0x04)|0x00));
        //write_register(0x4c,0x07); //0x07
        //write_register(0x4c,0x0e); //0x07
        delay_calib(33); 
        write_register(0x7b,((data_7bh&0x04)|0x03)); //Dsp_resetn_rx/tx  
        //write_register(0x4c,0x87);  //adc_pll reset
        //write_register(0x4c,0x8e); //0x07 
        

        if(3==rda_8850e_logic_version){
            write_register(0xe8,0x0d);//dev_3.0
            write_register(0x11,0x40); 
        }else if(4==rda_8850e_logic_version){
            write_register(0xe8,0x2f);//8850ew420_mipi
            write_register(0x11,0x00);
        }else if(5==rda_8850e_logic_version){
            write_register(0xe8,0x07);//8850ew420_mipi_v2000_6190_v1
            write_register(0x11,0x00);
        }else{
            write_register(0xe8,0x8c);//0c-->8c,0616 pa_on+lna
            write_register(0x11,0x80); 
        }       
        /*
        // ===============let's pll rx and tx to 10700!,0730============== HB
        //rx,0x90h,0x46h,0x48h,0xbch,0x4bh --------- > rf init 2140M
        write_register(0x92,0x49);
        write_register(0x4b,0x20);
        write_register(0xbd,0xa8);
        
        delay_calib(33);
        
        write_register(0x92,0xc9);//RXPLL_cal_resetn
        write_register(0x4b,0xa0);//sdm_resetn
        write_register(0xbd,0x28);//mdll_startup        
        //tx,0x3ah,0x42h,0x44h,0xbch,0x41h --------- > rf init 1950M        
        write_register(0x39,0x00);
        write_register(0x41,0x20);
        write_register(0xbd,0xa8);
    
        delay_calib(33);
        
        write_register(0x39,0x01);//TXPLL_cal_resetn    
        write_register(0x41,0xa0);//sdm_resetn  
        write_register(0xbd,0x28);//mdll_startup        
        // ============== end of pll reset ================================  
        */     
       
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
        //write_register(0x10,0x0f);
        //write_register(0x11,0xe8); 
        write_register(0x10,0x07);// backup switch of pll modem interface
        write_register(0x11,0x68);// backup switch of spll wd
        
        write_register(0x13,0x08);//notch_en  //// ------------- always close notch, at 0827  
        write_register(0x18,0x10);
        write_register(0x19,0x04);
        write_register(0xfe,0x00); //page=00

        write_register(0x7a,0x02);   
        write_register(0x7b,((data_7bh&0x04)|0x00));
        //write_register(0x4c,0x07); 
        //write_register(0x4c,0x0e); 
        delay_calib(33); 
        write_register(0x7b,((data_7bh&0x04)|0x03)); //Dsp_resetn_rx/tx
        //write_register(0x4c,0x87);
        //write_register(0x4c,0x8e);   
        
        if(3==rda_8850e_logic_version){
            write_register(0xe8,0x45);//dev_3.0
            write_register(0x11,0x00);
        }else if(4==rda_8850e_logic_version){
            write_register(0xe8,0x0f);//8850ew420_mipi
            write_register(0x11,0x00);
        }else if(5==rda_8850e_logic_version){
            write_register(0xe8,0x0f);//8850ew420_mipi_v2000_6190_v1
            write_register(0x11,0x00);            
        }else{
            write_register(0xe8,0x45);//0c-->8c,0616 pa_on
            write_register(0x11,0x40);    
        }     
    }   
    write_register(0xbd,0xa8);   
    delay_calib(33);        
    write_register(0xbd,0x28);//mdll_startup    
            
} 

void wcdma_narrow_bw_patch_on(void)
{
    
    write_register(0x1c,0x90);//gsm:filter_mode_gsm=1,filter_mode_td=0
    write_register(0x18,0xc0); //pga mode set
    
    write_register(0xfe,0x01);
    write_register(0x24,0x00);
    write_register(0xfe,0x00);
    write_register(0xd2,0x07);//6db
    
    write_register(0x19,0x80);//set ana calib gain
    write_register(0x1a,0x80); 
    write_register(0x6a,0x80);//set dig calib gain
    write_register(0x6b,0x00);
    write_register(0x6e,0x80);
    write_register(0x6f,0x00);
    write_register(0x70,0x00);
    write_register(0x72,0x0f);
    write_register(0x74,0x01);
    
    narrow_bw_search_flag=1;
}

void wcdma_narrow_bw_patch_off(void)
{
    
    write_register(0x1c,0x10);
    write_register(0x18,0x00); //pga mode set
    
    write_register(0xfe,0x01);
    write_register(0x24,0x80);
    write_register(0xfe,0x00);
    write_register(0xd2,0x47);//6db
    write_register(0x70,0x40);
    
    narrow_bw_search_flag=0;
}

short spll_count=1;
static void wcdma_spll_on(void)
{// backup switch
    
    short data_111h;
    
    write_register(0xfe,0x01);//page=01
    data_111h=read_register(0x11);
    write_register(0x11,(data_111h|0x80)); 
    write_register(0xfe,0x00);//page=01
    
    //write_register(0x4c,0x07); //0x07
    write_register(0x4c,0x0e); //0x07
    delay_calib(33); 
    //write_register(0x4c,0x87);  //adc_pll reset 
    write_register(0x4c,0x8e);  //adc_pll reset 
    
    write_register(0x77,spll_count++); 
}

static void wcdma_tx_on_br(void)
{
    short data_12h,data_13h;
    
    data_12h=read_register(0x12);
    data_13h=read_register(0x13);

    write_register(0x12,(data_12h|0x07));
    write_register(0x13,(data_13h|0x60));                 
}  

static void wcdma_tx_off_br(void)
{
    short data_12h,data_13h;
    
    data_12h=read_register(0x12);
    data_13h=read_register(0x13);

    write_register(0x12,(data_12h&0xf8));
    write_register(0x13,(data_13h&0x9f));                 
} 

void wcdma_rx_tx_off(void)//item:8850E_RF_OFF
{
    short data_12h,data_7bh;
    data_12h=read_register(0x12);
    data_7bh=read_register(0x7b);
    //write_register(0x12,0x80);//need change to 0x80 due to therm calib
    write_register(0x12,(data_12h&0x80)); 
    write_register(0x22,0x4b);//enlarge pll bandwidth to avoid freq shift
    write_register(0xe8,0x00);
    write_register(0x11,0x00);
    
    write_register(0x96,0x47);// Release reset of digrf buff
    write_register(0x7b,(data_7bh&0x04));// Release reset dsp  
    
    write_register(0x2f,0x48);
    write_register(0x2d,0x45);
     
    write_register(0xfe,0x01);//page=01
    //write_register(0x10,0x08);
    write_register(0x10,0x00);
    //write_register(0x11,0x80); //0x80
    write_register(0x11,0x00); //0x80
    write_register(0xfe,0x00); //page=00
    
    if(rda_8850e_tcxo){
        write_register(0x13,0x06);//vctcxo,0x4+pu_afc
    }else{
        write_register(0x13,0x00);//crystal
    }
}

static int dummy_read(int addr){
    int data=0;
    
#ifdef LABVIEW_TEST
    delay_calib(165);
#endif
    data=read_register(addr);
    return(data);
}

static void wcdma_rx_freq(void){
    
    int data_50h=0;//freq from BB
    int data_51h=0;//freq from BB
    int data_bch=0;//mdll_div_num
    int data_4bh=0;//mdll_div_num
    int data_46h=0;
    int rxpll_target_msb=0;
    int rxpll_target_remainder=0;
    int mdll_div_num;
    int data_50h_freq;
    
    data_bch = read_register(0xbc);
    data_4bh = read_register(0x4b);
    data_46h = read_register(0x46);
    data_50h = dummy_read(0x50);//must delay,otherwise value 0 is get
    data_51h = read_register(0x51);     
    
    data_50h_freq=((data_50h<<8)|data_51h)&0xffff;
    
    //check command's health
    if(0==data_50h_freq)
        return;  
    
    if(data_50h_freq>0x9664)//38500
    {
        band_select_flag=H_BAND;
    }else{
        band_select_flag=L_BAND;
    }
          
    if(H_BAND==band_select_flag)
    {
        //item:8850E_RF_ON_H
        write_register(0x14,0x5f);
        write_register(0x15,0x14);//54,0609                
        //write_register(0x1e,0x02);
        //write_register(0x1f,0x94);   

        write_register(0xfe,0x01);//page=01
        write_register(0x18,0x10);
        write_register(0x19,0x18);
        write_register(0xfe,0x00); //page=00 

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
        //write_register(0x1e,0x02);
        //write_register(0x1f,0x08);   

        write_register(0xfe,0x01);//page=01
        write_register(0x18,0x10);
        write_register(0x19,0x04);
        write_register(0xfe,0x00); //page=00
        
        if(3==rda_8850e_logic_version){
            write_register(0xe8,0x45);//dev_3.0
            //write_register(0xe8,0x61);//band 8
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
            //write_register(0xe8,0x61);//band 8
            write_register(0x11,0x40);    
        }     
    }    
    
    //mdll_div_num
    switch (data_50h_freq){
        case 200000:
        
        break;
        case 300000:
        
        break;
        default:
            mdll_div_num = 7;        
            write_register(0xbc,(data_bch&0x0f)|0x70);
            write_register(0x4b,(data_4bh&0xfc)|0x00);
    }  

    //freq calculation
    
    
    //txpll_target = ((data_50h_freq<<8)/26)&0xffff;
    rxpll_target_msb = ((data_50h_freq<<8)/(26*20))&0xffff;
    rxpll_target_remainder = ((data_50h_freq<<8)%(26*20))&0xffff;
    if(rxpll_target_remainder>((26*20)/2))
        rxpll_target_msb += 1;    
        
    
    int rx_freq_msb,rx_freq_lsb,rx_freq_remainder;
    rx_freq_msb = ((data_50h_freq<<8)/(mdll_div_num*26*20))&0xffff;
    rx_freq_remainder = (data_50h_freq<<8)%(mdll_div_num*26*20);    
    rx_freq_lsb = ((rx_freq_remainder<<16)/(mdll_div_num*26*20))&0xffff;
    
    int data01,data02,data03,data04,data05,data06;
    data01=(rxpll_target_msb>>8)&0xff;
    data02=(rxpll_target_msb)&0xff;
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
    //write_register(0xbd,0xa8);
    
    delay(156);
    
    write_register(0x92,0xc9);//RXPLL_cal_resetn
    write_register(0x4b,((mdll_div_num-7)&0x03)|0xa0);//sdm_resetn
    //write_register(0xbd,0x28);//mdll_startup

    last_band_select_flag=band_select_flag; 
}   

static void wcdma_tx_freq(void){
    
    int data_54h=0;//freq from BB
    int data_55h=0;//freq from BB
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
    data_54h = dummy_read(0x54);//must delay,otherwise value 0 is get
    data_55h = read_register(0x55);     
    
    data_54h_freq=((data_54h<<8)|data_55h)&0xffff;  
    
    //check command's health
    if(0==data_54h_freq)
        return;
 
    if(data_54h_freq>0x8fc0)//36800
    {
        band_select_flag=H_BAND;
    }else{
        band_select_flag=L_BAND;
    }
          
    if(H_BAND==band_select_flag)
    {
        //item:8850E_RF_ON_H           
        write_register(0x1e,0x02);
        write_register(0x1f,0x94);             
       
    }else{
        //item:8850E_RF_ON_L
        write_register(0x1e,0x02);
        write_register(0x1f,0x08);   
          
    }    
      
    //mdll_div_num
    switch (data_54h_freq){
        case 200000:
        
        break;
        case 300000:
        
        break;
        default:
            mdll_div_num = 7;        
            write_register(0xbc,(data_bch&0x0f)|0x70);
            write_register(0x41,(data_41h&0xfc)|0x00);
    }  

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
    //write_register(0xbd,0xa8);

    delay(156);
    
    write_register(0x39,0x01);//TXPLL_cal_resetn    
    write_register(0x41,((mdll_div_num-7)&0x03)|0xa0);//sdm_resetn  
    //write_register(0xbd,0x28);//mdll_startup
    
    if(H_BAND==band_select_flag)
    {
        write_register(0xf4,(((wcdma_apc_calib_ana_hb[0][0])&0xf00)>>8));
        write_register(0xf5,((wcdma_apc_calib_ana_hb[0][0])&0xff));
        write_register(0xfe,0x02);
        write_register(0x2c,(((wcdma_apc_calib_ana_hb[0][1])&0xf00)>>8));
        write_register(0x2b,((wcdma_apc_calib_ana_hb[0][1])&0xff));
        write_register(0xfe,0x00); 
    }else{
        write_register(0xf4,(((wcdma_apc_calib_ana_lb[0][0])&0xf00)>>8));
        write_register(0xf5,((wcdma_apc_calib_ana_lb[0][0])&0xff));
        write_register(0xfe,0x02);
        write_register(0x2c,(((wcdma_apc_calib_ana_lb[0][1])&0xf00)>>8));
        write_register(0x2b,((wcdma_apc_calib_ana_lb[0][1])&0xff));
        write_register(0xfe,0x00); 
    } 
} 



void gsm_wcdma_rf_initial(void){
    
    if(!logic_switch_flag){
        rda_8850e_logic_version=read_register(0xcf);
        logic_switch_flag=1;
    }
    if(!tcxo_read_flag){
        rda_8850e_tcxo=read_register(0x95);// 1:tcxo;0:crystal
        tcxo_read_flag=1;
    }
//    write_register(0xe8,0x08);//bs3_out for clock test temp!!!!!
    //write_register(0x30,0x50);
    //write_register(0x31,0x02); //register_retn=0,delay_50ms???
    //delay_100us();
    //write_register(0x30,0x50);
    //write_register(0x31,0x07); //register_retn=1    
    write_register(0x10,0xfe);
    write_register(0x11,0x00);
    write_register(0x12,0x00);    
    write_register(0x13,0x00);
    write_register(0x14,0x08);
    write_register(0x15,0x54); //lna_vbit lna_bias
    write_register(0x16,0xfa);
    write_register(0x17,0x84);
    write_register(0x18,0x00);    
    write_register(0x19,0x00); //pga_mode_gsm pga_mode_td
    write_register(0x1a,0x00);
    write_register(0x1b,0x08); //18--08,0619
    write_register(0x1c,0x10);    
    write_register(0x1d,0x88); //filter_mode_gsm filter_mode_td
    write_register(0x1e,0x02);
    write_register(0x1f,0x00);       
    write_register(0x20,0x88);
    write_register(0x21,0x81);
    write_register(0x22,0x4b);
    write_register(0x23,0x61);//changed on 0601 from hunan
    write_register(0x24,0x03);
    write_register(0x25,0x88);    
    write_register(0x26,0xff);
    write_register(0x27,0x45);//changed on 0601 from hunan
    write_register(0x28,0x80);//merge 80,e0-->80
    write_register(0x29,0x88);//therm    
    write_register(0x2a,0x20);
    write_register(0x2b,0xa0);
    write_register(0x2c,0xf8);
    write_register(0x2d,0x45);
    write_register(0x2e,0x3f);
    write_register(0x2f,0x48);//merge 4f,48--->4f ,0728--0x48  
    write_register(0x34,0xb6);//change clock 5 to 111
    write_register(0x35,0x6d);
    //write_register(0x36,0xbb);//change clock 7 to 011, 26M/128,7b--fb, change to 26m/8,fb-->bb 26m/32,bb-9b 26m/64, move to calib function
    write_register(0x3a,0x4b);    
    write_register(0x3b,0x00);//tx target pll,1950M,0730
    write_register(0x3c,0x00);    
    write_register(0x3d,0x00);//therm
    write_register(0x40,0x00);
    write_register(0x41,0xa0);
    write_register(0x42,0x2a);//merge 20,80-->20 // tx pll 1950M,0730
    write_register(0x43,0xb6);
    write_register(0x44,0xdb);    
    write_register(0x45,0x6d);
    write_register(0x46,0x8b);//rx pll 10700, 2140M,0730
    write_register(0x47,0xc2);
    write_register(0x48,0x1c);    
    write_register(0x49,0x21);
    write_register(0x4a,0x00);
    write_register(0x4b,0xa0);
    
    write_register(0x4c,0x8e);//merge 07,87-->87,0x8716d7d4,
    write_register(0x4d,0x2d); //spll=245.76MHz,0601,spll=368.64Mhz,0608
    write_register(0x4e,0xaf);     
    write_register(0x4f,0xa8); //spll=245.76MHz,0601,spll=368.64Mhz,0608
    /*
    write_register(0x4c,0x87);//merge 07,87-->87,0x8716d7d4,
    write_register(0x4d,0x16); //spll=245.76MHz,0601,spll=368.64Mhz,0608
    write_register(0x4e,0xd7);     
    write_register(0x4f,0xd4); //spll=245.76MHz,0601,spll=368.64Mhz,0608*/
    write_register(0x60,0x00);
    write_register(0x61,0xc0);
    //write_register(0x64,0x13); //0x12 dac test mode;0x18 bypass tx digital gain;0612
    write_register(0x64,0x00);
    write_register(0x65,0x20);
    write_register(0x68,0x00);    
    write_register(0x69,0x00);
    write_register(0x6a,0x00);
    write_register(0x6b,0x00);
    write_register(0x6c,0x00);    
    write_register(0x6d,0x00);
    write_register(0x6e,0x00);
    write_register(0x6f,0x00);
    write_register(0x70,0x5b);
    write_register(0x71,0x8f);
    write_register(0x72,0x05);     
    write_register(0x73,0x8b);
    write_register(0x74,0xfe);
    write_register(0x75,0xc3);
    
    //write_register(0x76,0x60);    
    //write_register(0x77,0x01);
    //write_register(0x78,0x20);
    //write_register(0x79,0x01);//occupied by therm value update, should not override when reinit
    
    write_register(0x7a,0x02);
    write_register(0x7b,0x03);
    write_register(0x90,0x52);
    write_register(0x91,0x4f);//rx_pll target at 2140M,0730    
    write_register(0x92,0xc9);    
    write_register(0x93,0x00);
    write_register(0x96,0xa7);
    write_register(0x97,0x08);//merge 08,00-->08
    write_register(0x98,0x00);    
    write_register(0x99,0x00);
    write_register(0x9a,0x00);
    write_register(0x9b,0x00);
    write_register(0x9c,0xff);
    write_register(0x9d,0xff);
    write_register(0x9e,0x00);     
    write_register(0x9f,0x42);
/*    write_register(0xa0,0xa1);
    write_register(0xa1,0x00);//take care!
    write_register(0xa4,0xdc);//0611
    write_register(0xa5,0x00);////added on 0601 from hunan
    write_register(0xa6,0x87);    
    write_register(0xa7,0x54);//---
    write_register(0xa8,0x80);
    write_register(0xa9,0x42);
    write_register(0xaa,0x91);
    write_register(0xab,0x00);// axh register will rewrite by modem!!!
*/
    //write_register(0xb0,0x64);    
   //write_register(0xb1,0x04);//624M setting
    write_register(0xb2,0x18);    
    write_register(0xb3,0x94);
    write_register(0xb4,0x04);
    write_register(0xb5,0x05);
    write_register(0xb6,0x04); //---change auto   
    write_register(0xb7,0x71); //---close test mode,0608
    
    //write_register(0xba,0x04);
    write_register(0xbb,0x01);//624M setting, add specially for 8850E U02
    
    write_register(0xbc,0x7c);
    write_register(0xbd,0x28);
    write_register(0xbe,0x61);     
    write_register(0xbf,0x0f);
    //write_register(0xc0,0x00);
    //write_register(0xc1,0x08);//624M setting 
    write_register(0xd0,0x00);
    write_register(0xd1,0x40);
    write_register(0xd2,0x47);//07-->47,gain0=6db,0710   
    write_register(0xd3,0x00);
    write_register(0xd6,0x09);//merge 09,20-->09
    write_register(0xd7,0x20);
    write_register(0xda,0x05);
    write_register(0xdb,0x04);
    write_register(0xde,0x0c);//merge 0c,00-->0c
    write_register(0xe0,0xa7);    
    write_register(0xe1,0x00);
    write_register(0xf0,0x06);    
    write_register(0xf1,0x34);
    write_register(0xf2,0x00);
    write_register(0xf3,0x00);
    write_register(0xf4,0x08); //merge 08,00-->08, 1213
    write_register(0xfa,0x00);
    write_register(0xfb,0x00);
    write_register(0xfe,0x01);//delay ? need verify        
    //write_register(0x10,0x08);
    //write_register(0x11,0x80);//07,20
    write_register(0x10,0x00);// backup switch of pll modem interface
    write_register(0x11,0x00);// backup switch of spll wd
    
    write_register(0x12,0x1f);//merge 1f,1a-->1f    
    write_register(0x13,0x18);//changed on 0601 from hunan,1f08,0611
    write_register(0x14,0x51);//power save 00:0x518c(wadc)//0x91a0
    write_register(0x15,0x8c);
    write_register(0x16,0x05);//power save 01:0x0544//0988
    write_register(0x17,0x44);
    write_register(0x18,0x80);    
    write_register(0x19,0x25);
    write_register(0x1a,0x10);
    write_register(0x1b,0x88);
    write_register(0x1c,0x28); //28   
    write_register(0x1d,0x1b);
    write_register(0x1e,0x40);
    write_register(0x1f,0x88);
    write_register(0x20,0x03);
    write_register(0x21,0xe4);
    write_register(0x22,0x04); //open refmulti2_en,1117, 0x04  
    //write_register(0x22,0x0c); //open refmulti2_en,0608, 0x0c   
    write_register(0x23,0x88);
    write_register(0x24,0x80);
    write_register(0x25,0x00);//set VGA gain to 6
    write_register(0x26,0x80);     
    write_register(0x27,0x25);
    write_register(0x28,0xe0);
    write_register(0x29,0x00);
    write_register(0x60,0xfc);
    write_register(0x61,0xd0);
    write_register(0x62,0x00);    
    write_register(0x63,0xc9);
    write_register(0x64,0x80);
    write_register(0x65,0x00);
    write_register(0x66,0xda);
    write_register(0x67,0x12);
    write_register(0x68,0x00);    
    write_register(0x69,0x00);
    write_register(0x82,0xa1);
    write_register(0x83,0x80); //digrf_rxen_delay_cnt<7:0>
    write_register(0x84,0x00);
    write_register(0x85,0x00);
    write_register(0x86,0x60);    
    write_register(0x87,0x00); //RX_notch1/0_bypass_WDTD
    
    write_register(0x90,0x80);
    write_register(0x91,0x00);
    write_register(0x92,0x40);
    write_register(0x93,0x00);
    write_register(0x94,0xe0);
    write_register(0x95,0x00);//therm
    
    write_register(0xb6,0xc9);    
    write_register(0xb7,0x00);    
    write_register(0xd8,0x00);    
    write_register(0xd9,0xff); //rc_bypass_tx/tx=1,0611,dac_clk,0701
    //write_register(0xe4,0x8c);    
    //write_register(0xe5,0x00);
    //write_register(0xe6,0x00);    
    //write_register(0xe7,0x00);
    //write_register(0xe8,0x10);    
    //write_register(0xe9,0x00);//624M setting
    
    //write_register(0xed,0x05);
    write_register(0xed,0x05);//clk_bb_en,clk_abb_en   
    write_register(0xfe,0x00);
    
    write_register(0xfe,0x02);
    write_register(0x20,0x40); //dac iq swap,0612,dac_sign
    write_register(0x2a,0x10); //gsm adc clk edge, merge 00-->10, 1213
    write_register(0x2c,0x08); //merge 00-->08, 1213
    write_register(0xfe,0x00);
   
}


void gsm_wcdma_rf_initial_d(void){
    
    write_register(0xfe,0x00);
    
    write_register(0x10,0xfe);
    write_register(0x11,0x00);
    write_register(0x12,0x00);    
    write_register(0x13,0x00);
    write_register(0x14,0x08);
    write_register(0x15,0x54); //lna_vbit lna_bias
    write_register(0x16,0xfa);
    write_register(0x17,0x84);
    write_register(0x18,0x00);    
    write_register(0x19,0x00); //pga_mode_gsm pga_mode_td
    write_register(0x1a,0x00);
    write_register(0x1b,0x08); //18--08,0619
    write_register(0x1c,0x10);    
    write_register(0x1d,0x88); //filter_mode_gsm filter_mode_td
    write_register(0x1e,0x02);
    write_register(0x1f,0x00);       
    write_register(0x20,0x88);
    write_register(0x21,0x81);
    write_register(0x22,0x4b);
    write_register(0x23,0x61);//changed on 0601 from hunan
    write_register(0x24,0x03);
    write_register(0x25,0x88);    
    write_register(0x26,0xff);
    write_register(0x27,0x45);//changed on 0601 from hunan
    write_register(0x28,0x80);//merge 80,e0-->80
    write_register(0x29,0x88);    
    write_register(0x2a,0x20);
    write_register(0x2b,0xa0);
    write_register(0x2c,0xf8);
    write_register(0x2d,0x45);
    write_register(0x2e,0x3f);
    write_register(0x2f,0x48);//merge 4f,48--->4f ,0728--0x48  
    write_register(0x34,0xb6);//change clock 5 to 111
    write_register(0x35,0x6d);
    //write_register(0x36,0xbb);//change clock 7 to 011, 26M/128,7b--fb, change to 26m/8,fb-->bb 26m/32,bb-9b 26m/64, move to calib function
    write_register(0x3a,0x4b);    
    write_register(0x3b,0x00);//tx target pll,1950M,0730
    write_register(0x3c,0x00);    
    write_register(0x3d,0x00);
    write_register(0x40,0x00);
    write_register(0x41,0xa0);
    write_register(0x42,0x2a);//merge 20,80-->20 // tx pll 1950M,0730
    write_register(0x43,0xb6);
    write_register(0x44,0xdb);    
    write_register(0x45,0x6d);
    write_register(0x46,0x8b);//rx pll 10700, 2140M,0730
    write_register(0x47,0xc2);
    write_register(0x48,0x1c);    
    write_register(0x49,0x21);
    write_register(0x4a,0x00);
    write_register(0x4b,0xa0);
    
    write_register(0x4c,0x8e);//merge 07,87-->87,0x8716d7d4,
    write_register(0x4d,0x2d); //spll=245.76MHz,0601,spll=368.64Mhz,0608
    write_register(0x4e,0xaf);     
    write_register(0x4f,0xa8); //spll=245.76MHz,0601,spll=368.64Mhz,0608
    /*
    write_register(0x4c,0x87);//merge 07,87-->87,0x8716d7d4,
    write_register(0x4d,0x16); //spll=245.76MHz,0601,spll=368.64Mhz,0608
    write_register(0x4e,0xd7);     
    write_register(0x4f,0xd4); //spll=245.76MHz,0601,spll=368.64Mhz,0608*/
    write_register(0x60,0x00);
    write_register(0x61,0xc0);
    //write_register(0x64,0x13); //0x12 dac test mode;0x18 bypass tx digital gain;0612
    write_register(0x64,0x00);
    write_register(0x65,0x20);
    write_register(0x68,0x00);    
    write_register(0x69,0x00);
    write_register(0x6a,0x00);
    write_register(0x6b,0x00);
    write_register(0x6c,0x00);    
    write_register(0x6d,0x00);
    write_register(0x6e,0x00);
    write_register(0x6f,0x00);
    write_register(0x70,0x5b);
    write_register(0x71,0x8f);
    write_register(0x72,0x05);     
    write_register(0x73,0x8b);
    write_register(0x74,0xfe);
    write_register(0x75,0xc3);
    write_register(0x76,0x60);    
    write_register(0x77,0x01);
    write_register(0x78,0x20);
    write_register(0x79,0x01);
    write_register(0x7a,0x02);
    write_register(0x7b,0x03);
    write_register(0x90,0x52);
    write_register(0x91,0x4f);//rx_pll target at 2140M,0730    
    write_register(0x92,0xc9);    
    write_register(0x93,0x00);
    write_register(0x96,0xa7);
    write_register(0x97,0x08);//merge 08,00-->08
    write_register(0x98,0x00);    
    write_register(0x99,0x00);
    write_register(0x9a,0x00);
    write_register(0x9b,0x00);
    write_register(0x9c,0xff);
    write_register(0x9d,0xff);
    write_register(0x9e,0x00);     
    write_register(0x9f,0x42);
/*    write_register(0xa0,0xa1);
    write_register(0xa1,0x00);//take care!
    write_register(0xa4,0xdc);//0611
    write_register(0xa5,0x00);////added on 0601 from hunan
    write_register(0xa6,0x87);    
    write_register(0xa7,0x54);//---
    write_register(0xa8,0x80);
    write_register(0xa9,0x42);
    write_register(0xaa,0x91);
    write_register(0xab,0x00);// axh register will rewrite by modem!!!
*/
    //write_register(0xb0,0x64);    
   //write_register(0xb1,0x04);//624M setting
    write_register(0xb2,0x18);    
    write_register(0xb3,0x94);
    write_register(0xb4,0x04);
    write_register(0xb5,0x05);
    write_register(0xb6,0x04); //---change auto   
    write_register(0xb7,0x71); //---close test mode,0608
    
    //write_register(0xba,0x04);
    write_register(0xbb,0x01);//624M setting, add specially for 8850E U02
    
    write_register(0xbc,0x7c);
    write_register(0xbd,0x28);
    write_register(0xbe,0x61);     
    write_register(0xbf,0x0f);
    //write_register(0xc0,0x00);
    //write_register(0xc1,0x08);//624M setting 
    write_register(0xd0,0x00);
    write_register(0xd1,0x40);
    write_register(0xd2,0x47);//07-->47,gain0=6db,0710   
    write_register(0xd3,0x00);
    write_register(0xd6,0x09);//merge 09,20-->09
    write_register(0xd7,0x20);
    write_register(0xda,0x05);
    write_register(0xdb,0x04);
    write_register(0xde,0x0c);//merge 0c,00-->0c
    write_register(0xe0,0xa7);    
    write_register(0xe1,0x00);
    write_register(0xf0,0x06);    
    write_register(0xf1,0x34);
    write_register(0xf2,0x00);
    write_register(0xf3,0x00);
    write_register(0xf4,0x08); //merge 08,00-->08, 1213
    write_register(0xfa,0x00);
    write_register(0xfb,0x00);

    
    write_register(0xfe,0x02);
    write_register(0x20,0x40); //dac iq swap,0612,dac_sign
    write_register(0x2a,0x10); //gsm adc clk edge, merge 00-->10, 1213
    write_register(0x2c,0x08); //merge 00-->08, 1213
    write_register(0xfe,0x00);
   
}


static void wcdma_rx_agc(void)
{
    unsigned short data_e8h,data_14h;
    unsigned short data_53h;
    unsigned char (*analog_p)[3];
    unsigned short index_ana=0xff;
    unsigned short power_index;
    unsigned char dig_len=0;
    unsigned char* volatile dig_end;
    unsigned char* volatile dig_pt;
    unsigned char data_dig=0xff;
    
    data_14h=read_register(0x14);
    data_e8h=read_register(0xe8);    
    data_53h = dummy_read(0x53);
    
    //data_53h = (((data_52h<<8)|data_53h)&0xffff)>>4;
    power_index = (data_53h>wcdma_agc_index[0])?(wcdma_agc_index[0]):((data_53h<wcdma_agc_index[WCDMA_AGC_HB_ANA_NUM])?(wcdma_agc_index[WCDMA_AGC_HB_ANA_NUM]):(data_53h));

    for(short i=0;i<WCDMA_AGC_HB_ANA_NUM;i++){
        if(((wcdma_agc_index[i])>=power_index)&&((wcdma_agc_index[i+1])<power_index)){
            index_ana = i;
            break;
        }
    }            
    if(0xff==index_ana)
        return;

    if(H_BAND==band_select_flag)
    {
        //item:8850E_RF_ON_H
        if(3==rda_8850e_logic_version){
            if(index_ana<WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,((data_e8h&0xf0)|(0x0d)));//H gain
            }else{
                write_register(0xe8,((data_e8h&0xf0)|(0x0f)));//M gain
            } 
        }else if(4==rda_8850e_logic_version){
            if(index_ana<WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x2f);//H gain
            }else{
                write_register(0xe8,0x2d);//M gain
            } 
        }else if(5==rda_8850e_logic_version){
            if(index_ana<WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x07);//H gain,8850ew420_mipi_v2000_6190_v1
            }else{
                write_register(0xe8,0x05);//M gain,8850ew420_mipi_v2000_6190_v1
            }             
        }else{      
            if(index_ana<WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,((data_e8h&0xf0)|(0x0c)));//H gain
            }else{
                write_register(0xe8,((data_e8h&0xf0)|(0x0e)));//M gain
            }
        }          

/*         
        if(index_ana<2){
            write_register(0x17,0x94);//pga_bw_rbit=0b10
        }else{
            write_register(0x17,0x84);//pga_bw_rbit=0b00
        }  
*/
        dig_pt = wcdma_agc_hb_table_digital_gain_pt[index_ana];
        if(narrow_bw_search_flag){
            if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
                dig_pt = wcdma_agc_hb_table_digital_gain_pt_ln41_nbw[index_ana];
            }else{
                dig_pt = wcdma_agc_hb_table_digital_gain_pt_nbw[index_ana];
            }
        }

        dig_len = wcdma_agc_hb_table_length[index_ana];
        dig_end = dig_pt + dig_len;
        for(;dig_pt<dig_end;dig_pt++){
            if(power_index==*dig_pt){
                data_dig=*(dig_pt+dig_len);
                break;
            }
        }
        
        
        analog_p = wcdma_agc_hb_table_analog;
        wcdma_agc_calib_ana = wcdma_agc_calib_ana_hb;
        wcdma_agc_calib_dig = wcdma_agc_calib_dig_hb;
        
    }else{  

        if(3==rda_8850e_logic_version){
            if(index_ana<WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,((data_e8h&0xf0)|(0x05)));//H gain
            }else{
                write_register(0xe8,((data_e8h&0xf0)|(0x07)));//M gain
            }  
        }else if(4==rda_8850e_logic_version){
            if(index_ana<WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x0f);//H gain
            }else{
                write_register(0xe8,0x0d);//M gain
            } 
        }else if(5==rda_8850e_logic_version){
            if(index_ana<WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,0x0f);//H gain,8850ew420_mipi_v2000_6190_v1
            }else{
                write_register(0xe8,0x0d);//M gain,8850ew420_mipi_v2000_6190_v1
            }             
        }else{
            if(index_ana<WCDMA_AGC_LNA_GAIN_SWITCH){ 
                write_register(0xe8,((data_e8h&0xf0)|(0x05)));//H gain
            }else{
                write_register(0xe8,((data_e8h&0xf0)|(0x07)));//M gain
            } 
        }    

/*        
        if(index_ana<2){
            write_register(0x17,0x94);//pga_bw_rbit=0b10
        }else{
            write_register(0x17,0x84);//pga_bw_rbit=0b00
        }  
*/   
        dig_pt = wcdma_agc_hb_table_digital_gain_pt[index_ana];
        if(narrow_bw_search_flag){
            if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
                dig_pt = wcdma_agc_hb_table_digital_gain_pt_ln41_nbw[index_ana];
            }else{
                dig_pt = wcdma_agc_hb_table_digital_gain_pt_nbw[index_ana];
            }
        }

        dig_len = wcdma_agc_hb_table_length[index_ana];
        dig_end = dig_pt + dig_len;
        for(;dig_pt<dig_end;dig_pt++){
            if(power_index==*dig_pt){
                data_dig=*(dig_pt+dig_len);
                break;
            }
        }
                            
        analog_p = wcdma_agc_hb_table_analog;
        wcdma_agc_calib_ana = wcdma_agc_calib_ana_lb;
        wcdma_agc_calib_dig = wcdma_agc_calib_dig_lb;    
    }  
    if(narrow_bw_search_flag){       
        if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
            analog_p = wcdma_agc_hb_table_analog_ln41_nbw;
        }else{
            analog_p = wcdma_agc_hb_table_analog_nbw;
        }
    }else if((4==rda_8850e_logic_version)||(5==rda_8850e_logic_version)){
        analog_p = wcdma_agc_hb_table_analog_ln41;
    }

    write_register(0x14,((data_14h&0xf0)|((*(analog_p+index_ana))[2]))); 
    write_register(0x16,((*(analog_p+index_ana))[0]));
    write_register(0x10,((*(analog_p+index_ana))[1]));      
    write_register(0xd7,data_dig);

    if(!narrow_bw_search_flag){
        write_register(0x19,((*(wcdma_agc_calib_ana+index_ana))[0]));//set ana calib gain
        write_register(0x1a,((*(wcdma_agc_calib_ana+index_ana))[1])); 
    
        write_register(0x6a,((*(wcdma_agc_calib_dig+index_ana))[0]));//set dig calib gain
        write_register(0x6b,((*(wcdma_agc_calib_dig+index_ana))[1]));
        write_register(0x6e,((*(wcdma_agc_calib_dig+index_ana))[2]));
        write_register(0x6f,((*(wcdma_agc_calib_dig+index_ana))[3]));
    }
    
}

static void wcdma_tx_gain(void)
{   
    short data_56h;
    short data_57h;
    unsigned short power_56h=0;
    unsigned short power_index=0;
    unsigned short dig_len=0;
    unsigned short* volatile dig_end;
    unsigned short* volatile dig_pt;
    unsigned short index_ana=0xff;
    short i;
    unsigned short data_ana_0,data_ana_1,data_ana_2,data_dig;
    
    data_56h=dummy_read(0x56); 
    data_57h=read_register(0x57);

    power_56h = (((data_56h<<8)|data_57h)&0xffff)>>4;
    
    power_index = (power_56h>wcdma_apc_hb_index[0])?(wcdma_apc_hb_index[0]):((power_56h<wcdma_apc_hb_index[CALIB_WCDMA_TX_ANA_GAIN_NUM])?(wcdma_apc_hb_index[CALIB_WCDMA_TX_ANA_GAIN_NUM]):(power_56h));

    for(i=0;i<CALIB_WCDMA_TX_ANA_GAIN_NUM;i++){
        if((power_index<=wcdma_apc_hb_index[i])&&(power_index>wcdma_apc_hb_index[i+1])){
            index_ana = i;
            break;
        }
    }
    
    if(0xff==index_ana)
        return;

    data_ana_0 = wcdma_apc_hb_table_ana_gain_0[index_ana];
    data_ana_1 = wcdma_apc_hb_table_ana_gain_1[index_ana];
    data_ana_2 = wcdma_apc_hb_table_ana_gain_2[index_ana];
      
    if(H_BAND==band_select_flag)
    {
        //item:8850E_RF_ON_H
        dig_pt = wcdma_apc_hb_table_digital_gain_pt[index_ana];
        dig_len = wcdma_apc_hb_table_length[index_ana];
        dig_end = dig_pt + dig_len;
    }else{  
        //item:8850E_RF_ON_L
        dig_pt = wcdma_apc_lb_table_digital_gain_pt[index_ana];
        dig_len = wcdma_apc_hb_table_length[index_ana];
        dig_end = dig_pt + dig_len; 
    }  
    
    for(;dig_pt<dig_end;dig_pt++){
        if(power_index==*dig_pt){
            data_dig=*(dig_pt+dig_len);
            break;
        }
    }
    
    if((ThermComp.TxGainState&0xffff)==0xff30)
    {
        data_dig = wcdma_tx_comp(data_dig,ThermTemp.TxGainOffset);
        write_register(0x78,(ThermTemp.TxGainOffset>>8)); 
        write_register(0x79,(ThermTemp.TxGainOffset));        
    }
    write_register(0x2e,data_ana_0);
    write_register(0x2a,data_ana_1);
    write_register(0x2d,data_ana_2);
    write_register(0x66,((data_dig>>8)&0xe0));
    write_register(0x67,(data_dig&0xff));  
   
}   

unsigned short wcdma_tx_comp(unsigned short base, short offset)
{
    short ct1,ct2,size;
    
    ct1 = (base&0xe000)>>13;
    ct2 = (base&0x7f);
    
    if(offset&0x8000){// negative
        offset = -offset;
        if(offset>ct2){
            size = 1+((offset-ct2)/0x78);
            ct1 -= 1*size;
            ct2 = (ct2+0x78*size-offset);
        }else{
            ct2 = ct2-offset;
        }
    }else{
        if((ct2+offset)>0x7f){
            size = (ct2+offset)/0x78;
            ct1 += 1*size;
            ct2 = (ct2+offset-0x78*size);
        }else{
            ct2 = ct2+offset;
        }                
    }
    ct1 = ct1>0x7?0x7:(ct1<0x0?0x0:ct1);
    ct2 = ct2>0x7f?0x7f:(ct2<0x0?0x0:ct2);
    return(((ct1&0x7)<<13)|(ct2&0x7f));
}     

static void wcdma_rx_freq_target(int rxfreq){
    
    int data_bch=0;//mdll_div_num
    int data_4bh=0;//mdll_div_num
    int data_46h=0;
    int rxpll_target_msb=0;
    int rxpll_target_remainder=0;
    int mdll_div_num;
    int data_50h_freq = rxfreq;
    
    data_bch = read_register(0xbc);
    data_4bh = read_register(0x4b);
    data_46h = read_register(0x46);
     
    //check command's health
    if(0==data_50h_freq)
        return;    
    
    mdll_div_num = 7;        
    write_register(0xbc,(data_bch&0x0f)|0x70);
    write_register(0x4b,(data_4bh&0xfc)|0x00);

    if(data_50h_freq>0x9664)//38500
    {
        band_select_flag=H_BAND;
    }else{
        band_select_flag=L_BAND;
    }
          
    if(H_BAND==band_select_flag)
    {
        //item:8850E_RF_ON_H
        write_register(0x14,0x5f);
        write_register(0x15,0x14);//54,0609                
        //write_register(0x1e,0x02);
        //write_register(0x1f,0x94);   

        write_register(0xfe,0x01);//page=01
        write_register(0x18,0x10);
        write_register(0x19,0x18);
        write_register(0xfe,0x00); //page=00 

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
        //write_register(0x1e,0x02);
        //write_register(0x1f,0x08);   

        write_register(0xfe,0x01);//page=01
        write_register(0x18,0x10);
        write_register(0x19,0x04);
        write_register(0xfe,0x00); //page=00
        
        if(3==rda_8850e_logic_version){
            write_register(0xe8,0x45);//dev_3.0
            //write_register(0xe8,0x61);//band 8
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
            //write_register(0xe8,0x61);//band 8
            write_register(0x11,0x40);    
        }     
    } 
    //freq calculation
    
    
    //txpll_target = ((data_50h_freq<<8)/26)&0xffff;
    rxpll_target_msb = ((data_50h_freq<<8)/(26*20))&0xffff;
    rxpll_target_remainder = ((data_50h_freq<<8)%(26*20))&0xffff;
    if(rxpll_target_remainder>((26*20)/2))
        rxpll_target_msb += 1;    
        
    
    int rx_freq_msb,rx_freq_lsb,rx_freq_remainder;
    rx_freq_msb = ((data_50h_freq<<8)/(mdll_div_num*26*20))&0xffff;
    rx_freq_remainder = (data_50h_freq<<8)%(mdll_div_num*26*20);    
    rx_freq_lsb = ((rx_freq_remainder<<16)/(mdll_div_num*26*20))&0xffff;
    
    int data01,data02,data03,data04,data05,data06;
    data01=(rxpll_target_msb>>8)&0xff;
    data02=(rxpll_target_msb)&0xff;
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
    //write_register(0xbd,0xa8);
    
    delay(156);
    
    write_register(0x92,0xc9);//RXPLL_cal_resetn
    write_register(0x4b,((mdll_div_num-7)&0x03)|0xa0);//sdm_resetn
    //write_register(0xbd,0x28);//mdll_startup
    last_band_select_flag=band_select_flag;
} 

static void wcdma_tx_freq_target(int txfreq){
    
    int data_bch=0;//mdll_div_num
    int data_41h=0;//mdll_div_num
    int data_42h=0;
    int txpll_target_msb=0;
    int txpll_target_remainder=0;
    int mdll_div_num;
    int data_54h_freq = txfreq;
    
  
    data_bch = read_register(0xbc);
    data_41h = read_register(0x41);
    data_42h = read_register(0x42);
       
    //check command's health
    if(0==data_54h_freq)
        return;
    
    mdll_div_num = 7;        
    write_register(0xbc,(data_bch&0x0f)|0x70);
    write_register(0x41,(data_41h&0xfc)|0x00);
    
    if(data_54h_freq>0x8fc0)//36800
    {
        band_select_flag=H_BAND;
    }else{
        band_select_flag=L_BAND;
    }
          
    if(H_BAND==band_select_flag)
    {
        //item:8850E_RF_ON_H           
        write_register(0x1e,0x02);
        write_register(0x1f,0x94);             
       
    }else{
        //item:8850E_RF_ON_L
        write_register(0x1e,0x02);
        write_register(0x1f,0x08);   
          
    }  
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
    //write_register(0xbd,0xa8);

    delay(156);
    
    write_register(0x39,0x01);//TXPLL_cal_resetn    
    write_register(0x41,((mdll_div_num-7)&0x03)|0xa0);//sdm_resetn  
    //write_register(0xbd,0x28);//mdll_startup
    if(H_BAND==band_select_flag)
    {
        write_register(0xf4,(((wcdma_apc_calib_ana_hb[0][0])&0xf00)>>8));
        write_register(0xf5,((wcdma_apc_calib_ana_hb[0][0])&0xff));
        write_register(0xfe,0x02);
        write_register(0x2c,(((wcdma_apc_calib_ana_hb[0][1])&0xf00)>>8));
        write_register(0x2b,((wcdma_apc_calib_ana_hb[0][1])&0xff));
        write_register(0xfe,0x00); 
    }else{
        write_register(0xf4,(((wcdma_apc_calib_ana_lb[0][0])&0xf00)>>8));
        write_register(0xf5,((wcdma_apc_calib_ana_lb[0][0])&0xff));
        write_register(0xfe,0x02);
        write_register(0x2c,(((wcdma_apc_calib_ana_lb[0][1])&0xf00)>>8));
        write_register(0x2b,((wcdma_apc_calib_ana_lb[0][1])&0xff));
        write_register(0xfe,0x00); 
    }   
}  

void wcdma_irat_switch(void)
{
    int data_5ch,data_5dh,ivalue;
    int rx_freq,tx_freq;
    
    data_5ch = dummy_read(0x5c); 
    data_5dh = read_register(0x5d);

    ivalue = ((data_5ch<<8)|data_5dh)&0xffff;
    
    if(0 == ivalue){
        wcdma_rx_tx_off();      
        last_rf_state = 5;
    }else if((ivalue >= 0xa508) && (ivalue <= 0xa958)){
        wcdma_rx_tx_on();
        wcdma_tx_on_br();
        
        rx_freq = ivalue;
        tx_freq = (ivalue - 950*4);
        wcdma_rx_freq_target(rx_freq);
        wcdma_tx_freq_target(tx_freq);       
        last_rf_state = 6;
    }
}
     
// 7
void wcdma_rf_initial_isr()
{
    //*WADDR_EXT_REG = 0xff00e804;
    //*WADDR_EXT_REG = 0xdf00e804;
    response_bb(7,5,(0xa0));
    
    clear_interrupt(0x05,0x80);// enable current interrupt 
    
    gsm_wcdma_rf_initial(); 
    
    response_bb(7,5,(0x40));  
    //*WADDR_EXT_REG = 0xff00e800;
    //*WADDR_EXT_REG = 0xdf00e800;
}


// 6
void wcdma_rf_on_isr()
{

    last_rf_state = 6;
    response_bb(6,5,(0xa0)); 
    clear_interrupt(0x05,0x40);
    
    wcdma_rx_tx_on();
    
    ThermFsm.WPathWorking = TRUE; 
    
    response_bb(6,5,(0x40)); 

}

// 5
void wcdma_rf_off_isr()
{

    last_rf_state = 5;
    response_bb(5,5,(0xa0));
    clear_interrupt(0x05,0x20);
    
    wcdma_rx_tx_off();
    
    therm_value_update();//therm value update 01 when w modem sleep[00-01]
    ThermFsm.WPathWorking = FALSE;
    
    response_bb(5,5,(0x40)); 

}

// 4
void wcdma_tx_on_isr()
{

    response_bb(4,5,(0xa0));
    clear_interrupt(0x05,0x10);
    
    wcdma_tx_on_br();
    response_bb(4,5,(0x40));
}

// 3
void wcdma_tx_off_isr()
{
    response_bb(3,5,(0xa0));
    clear_interrupt(0x05,0x08);

    wcdma_tx_off_br();
    response_bb(3,5,(0x40));   
}
//0x50/0x51  15
void wcdma_rx_freq_hw_isr()
{

    response_bb(15,5,(0xa0));
    clear_interrupt(0x04,0x80);
      
    wcdma_rx_freq();
    
    hw_int_accum(15);
    response_bb(15,5,(0x40));
}


//0x52/0x59  14
void wcdma_rx_gain_hw_isr()
{

    response_bb(14,5,(0xa0));
    clear_interrupt(0x04,0x40);
    
    wcdma_rx_agc();
    
    hw_int_accum(14);
    response_bb(14,5,(0x40)); 
}


//0x54/0xd1  13
void wcdma_tx_freq_hw_isr()
{

    response_bb(13,5,(0xa0));
    clear_interrupt(0x04,0x20);
      
    wcdma_tx_freq();
    
    hw_int_accum(13);
    response_bb(13,5,(0x40));

}

//0x56/0xe1  12
void wcdma_tx_pwr_hw_isr()
{

    response_bb(12,5,(0xa0));
    clear_interrupt(0x04,0x10);

    wcdma_tx_gain();  
    
    hw_int_accum(12);
    response_bb(12,5,(0x40));

}
//0x5e
void wcdma_spll_hw_isr()
{
    response_bb(8,5,(0xa0));
    short data_5eh,data_5fh;
    
    clear_interrupt(0x04,0x01);

    data_5eh = dummy_read(0x5e); 
    data_5fh = read_register(0x5f);
    
    data_5eh = (((data_5eh<<8)|data_5fh)&0xffff);
 
    if(0x0 == data_5eh){
        wcdma_spll_on();
    }else if(0x02 == data_5eh){
        wcdma_narrow_bw_patch_on();
    }else if(0x03 == data_5eh){
        wcdma_narrow_bw_patch_off();
    }
    response_bb(8,5,(0x40));
}

//0x5c
void wcdma_irat_hw_isr()
{

    response_bb(9,5,(0xa0));
    clear_interrupt(0x04,0x02);

    wcdma_irat_switch();  
    
    response_bb(9,5,(0x40));
}
