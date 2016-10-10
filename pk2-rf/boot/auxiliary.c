#include <rf/register.h>
#include <rf/calib_wcdma.h>
#include <rf/auxiliary.h>

/* external therm calib data definition */

Therm_State_T ThermFsm = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,{FALSE,FALSE},};
Therm_Calib_T ThermComp;
Therm_TxTemp_T ThermTemp = {0,{14420,14675,14910,15124,15350,15575,15790,16022,16234,16466,16694},0,0};

/* internal therm calib data definition */
short therm_value[11] = {-20,-10,0,10,20,30,40,50,60,70,80};
short therm_tempe[11] = {13983,14189,14401,14603,14806,15029,15246,15455,15689,15911,16145};// Room temperature
short therm_temIn[11] = {14420,14675,14910,15124,15350,15575,15790,16022,16234,16466,16694};// Inner temperature

short therm_agc_ana_offset[CALIB_WCDMA_RX_ANA_GAIN_NUM][11] = {{2,2,1,1,0,0,-1,-2,-2,-3,-4},{2,2,1,1,0,0,0,-1,-2,-2,-3},{2,2,1,1,0,0,0,-1,-2,-2,-3},
                                            {2,1,1,0,0,0,0,-1,-1,-2,-3},{2,1,1,0,0,0,0,-1,-1,-2,-3},{2,1,1,0,0,0,0,-1,-1,-2,-3},{1,1,1,0,0,0,0,-1,-1,-1,-2}};
short therm_apc_ana_offset[CALIB_WCDMA_TX_ANA_GAIN_NUM] = {0,1,2,3,3,4,4,5,5,5,5};

void therm_state_init(void)
{
   
    ThermFsm.ThermWorking = FALSE;
    ThermFsm.ThermUseable = FALSE;
    ThermFsm.PwdWorking = FALSE;
    ThermFsm.PwdUseable = FALSE;
    ThermFsm.WPathWorking = FALSE;
    ThermFsm.GPathWorking = FALSE; 
    ThermFsm.InnerState.StateOpen = FALSE;
    ThermFsm.InnerState.StateCopy = FALSE;
    
}

void therm_internal_trigger(void) property(loop_levels_0)
{
    short data_12h,data_13h,data_7bh;
    
    if(TRUE == ThermFsm.PwdWorking){
        return;
    }
    ThermFsm.ThermWorking = TRUE;
    
    data_12h = read_register(0x12);
    data_13h = read_register(0x13);
    data_7bh = read_register(0x7b);
    
    write_register(0x12,(data_12h|0x80)); 
    write_register(0x13,(data_13h|0x04));//pu_thmt
    write_register(0x3c,0x20);//thmt_clk=1
    //write_register(0x3d,0x00);//thmt_clk=1
    write_register(0x28,0x80);//0x80 inner therm,0x88 outter therm    
    write_register(0x29,0x88);//thmt_beta_indep_en=0,
    write_register(0x36,0xb7);
    write_register(0xfe,0x01); 
    write_register(0x2a,0x80);
    write_register(0x2b,0x00);
    write_register(0xfe,0x00);
        
    write_register(0x27,0x44);
    write_register(0x7b,(data_7bh&0x03));
    delay_calib(33);
    write_register(0x27,0x45);
    write_register(0x7b,(data_7bh|0x04));
}
  
void therm_pwd_trigger(void)
{
    short data_12h,data_13h,data_7bh,data_111h;
    
    data_12h = read_register(0x12);
    data_13h = read_register(0x13);
    data_7bh = read_register(0x7b);
    write_register(0xfe,0x01); 
    data_111h = read_register(0x11);
    write_register(0xfe,0x00);
    
    write_register(0x12,(data_12h|0x80)); 
    write_register(0x13,(data_13h|0x04));//pu_thmt
    write_register(0x3c,0x20);//thmt_clk=1
    //write_register(0x3d,0x00);//thmt_clk=1
    write_register(0x28,0x84);//0x80 inner therm,0x88 outter therm    
    write_register(0x29,0x88);//thmt_beta_indep_en=0,
    write_register(0x36,0xb7);
    write_register(0xfe,0x01);
    write_register(0x11,(data_111h|0x20)); 
    write_register(0x2a,0x80);
    write_register(0x2b,0x00);
    write_register(0xfe,0x00);
        
    write_register(0x7b,(data_7bh&0x03));
    delay_calib(33);
    write_register(0x7b,(data_7bh|0x04));
}

void therm_value_update(void) property(loop_levels_0)
{
    short data_1f4h,data_1f5h;
    
    if(FALSE == ThermFsm.ThermWorking){
        return;
    }
    ThermFsm.ThermUseable = TRUE;
    
    write_register(0xfe,0x01);
    data_1f4h=read_register(0xf4);
    data_1f5h=read_register(0xf5);
    write_register(0xfe,0x00);
    
    // carry to page 0
    write_register(0x76,(data_1f4h&0x7f));
    write_register(0x77,data_1f5h);
}

void therm_internal_close(void)
{
    short data_12h;
    
    data_12h=read_register(0x12);
    write_register(0x12,(data_12h&0x7f));// close therm
    
    ThermFsm.ThermWorking=FALSE;
    ThermFsm.ThermUseable=FALSE; 
}

short bisearch(short buf[], short target, short beg, short las)
{
    short mid = -1;
    
    if(target <= buf[beg])
        return (beg);
    else if(target >= buf[las])
        return (las);
    
    while((beg+1) < las){
        mid = (beg + las) / 2;
        if(target < buf[mid]){
            las = mid;
        }else if(target >= buf[mid]){
            beg = mid;
        }
    }
    return (beg);
}

short therm_internal_heat(short thermindication)
{
    short startindex, temperature, tempediff = 10;
    char begin = 0, last = 10;
    
    if(thermindication <= therm_tempe[begin]){
        temperature = therm_value[begin] - (((therm_tempe[begin]-thermindication)*tempediff)/(therm_tempe[begin+1]-therm_tempe[begin]));
    }else if(thermindication >= therm_tempe[last]){
        temperature = therm_value[last] + (((thermindication-therm_tempe[last])*tempediff)/(therm_tempe[last]-therm_tempe[last-1]));
    }else{
        startindex = bisearch(therm_tempe,thermindication,begin,last);
        temperature = therm_value[startindex] + (((thermindication-therm_tempe[startindex])*tempediff)/(therm_tempe[startindex+1]-therm_tempe[startindex]));
    }
    return (temperature);
}

short therm_internal_rxgain(short anagain, short digigain)
{ 
    short data_1f4h,data_1f5h;
    short thermvalue, sindex, finalgain;
    
    write_register(0xfe,0x01);
    data_1f4h=read_register(0xf4);
    data_1f5h=read_register(0xf5);
    write_register(0xfe,0x00);
    
    thermvalue = (((data_1f4h&0x7f)<<8)|(data_1f5h&0xff));
    sindex = bisearch(therm_tempe,thermvalue,0,10);
    finalgain = digigain + therm_agc_ana_offset[anagain][sindex];
    finalgain = finalgain<0x7f?(finalgain<0?0:finalgain):0x7f;
    return (finalgain);
}

short mipi_count=1;
inline void mipi_clock_simu(void){
    *WADDR_EXT_REG = 0xff00e804;
    *WADDR_EXT_REG = 0xdf00e804;
 
    *WADDR_EXT_REG = 0xff001304;
    *WADDR_EXT_REG = 0xdf001304;//power on  
    for(int i=0;i<0xffff;i++)
    {
        *WADDR_EXT_REG = 0xff00e808;
        *WADDR_EXT_REG = 0xdf00e808;        
        *WADDR_EXT_REG = 0xff00e800;
        *WADDR_EXT_REG = 0xdf00e800;    
    }
    *WADDR_EXT_REG = 0xff001300;
    *WADDR_EXT_REG = 0xdf001300;//power off  
    
    *WADDR_EXT_REG = 0xff00e800;
    *WADDR_EXT_REG = 0xdf00e800;
    write_register(0xcd,mipi_count++);
}

void thermometer_cycle(void) property(loop_levels_0)
{
    therm_internal_trigger();
    delay_calib(33000*2);
    //delay_calib(16500);
    therm_value_update();
    
    therm_internal_close();
}

void ThermTxTempUpdate(void)
{
    short i;
    for(i=0;i<11;i++){
        ThermTemp.TxTempReset[i] = therm_temIn[i] + (ThermComp.TxCalibTemp - therm_temIn[5]);
    }
    ThermTemp.TxTempState = 1;
}

void ThermTxComp(short CurrHeat)
{

    short StartNum, TuneGain, *ThermGain, *ThermTempBuff, GainOffset;
       
    if(!ThermTemp.TxTempState){
        ThermTxTempUpdate();
    }
    ThermGain = (short*)&(ThermComp.TxGainComp);
    ThermTempBuff = (short*)&(ThermTemp.TxTempReset);
    if(CurrHeat <= (*ThermTempBuff)){
            GainOffset = (short)(*ThermGain);
    }else if(CurrHeat >= (*(ThermTempBuff+10))){
            GainOffset = (short)(*(ThermGain+10));
    }else{
            StartNum = bisearch(ThermTempBuff,CurrHeat,0,10);
            TuneGain = *(ThermGain+StartNum) + ((CurrHeat-(*(ThermTempBuff+StartNum)))*(*(ThermGain+StartNum+1)-*(ThermGain+StartNum)))/(*(ThermTempBuff+StartNum+1)-*(ThermTempBuff+StartNum));
            GainOffset = (short)(TuneGain);
    }
    ThermTemp.TxGainOffset = GainOffset;
    ThermTemp.TxCurrTemper = CurrHeat;			
}

inline assembly void sendsequence(int* chess_storage(r9) se, int chess_storage(r8) wr) clobbers(r0,r8,r9) property(loop_levels_1)
{
    asm_begin
        do 25,12
        nop
        nop
        lwp r0,(r9)
        sw r0,(r8,0)
        nop
        lwp r0,(r9)
        sw r0,(r8,0)
        nop
        lwp r0,(r9)
        sw r0,(r8,0)
        nop
        lwp r0,(r9)
        sw r0,(r8,0)
        nop
    
    asm_end
}


void mipi_rffe_register_write(char sa, char address, char data) property(loop_levels_1)// 25 bits
{
    unsigned char dataparity=1,senddata=data;
    unsigned short addrparity=1,sendaddr;
    int command;
    int sequence[25+1][4];
    
    sendaddr = (((sa&0xf)<<8)|(2<<5)|(address&0x1f))&0xfff;
    while(sendaddr){
        addrparity += sendaddr&0x1;
        sendaddr >>= 1;
    }
    addrparity = addrparity&0x1;
    
    while(senddata){
        dataparity += senddata&0x1;
        senddata >>= 1;
    }
    dataparity = dataparity&0x1;
    
    command = ((2<<23)|((sa&0xf)<<19)|(2<<16)|((address&0x1f)<<11)|(addrparity<<10)|((data&0xff)<<2)|((dataparity<<1)))&0x1fffffe;
    
    for(char i=0;i<25;i++)
    {
        if(i<2){
            sequence[i][0] = 0xff001100|(((((command>>(24-i))&0x1)<<6)|(0x00))&0xff);
            sequence[i][1] = 0xdf001100|(((((command>>(24-i))&0x1)<<6)|(0x00))&0xff);
            sequence[i][2] = 0xff001100|(((((command>>(24-i))&0x1)<<6)|(0x00))&0xff);
            sequence[i][3] = 0xdf001100|(((((command>>(24-i))&0x1)<<6)|(0x00))&0xff);
        }else{
            sequence[i][0] = 0xff001100|(((((command>>(24-i))&0x1)<<6)|(0x80))&0xff);
            sequence[i][1] = 0xdf001100|(((((command>>(24-i))&0x1)<<6)|(0x80))&0xff);
            sequence[i][2] = 0xff001100|(((((command>>(24-i))&0x1)<<6)|(0x00))&0xff);
            sequence[i][3] = 0xdf001100|(((((command>>(24-i))&0x1)<<6)|(0x00))&0xff);
        }
    }
    
    sendsequence((int*)&sequence,0x3fff0);
}


//0x58
void wcdma_therm_hw_isr()
{
    response_bb(11,5,(0xa0));
    clear_interrupt(0x04,0x08);

    short data_59h;
    data_59h=read_register(0x59);
    if(0x01 == data_59h){
        therm_internal_trigger();
    }else if(0x02 == data_59h){
        therm_pwd_trigger();
    }else if(0x03 == data_59h){
        if(ThermFsm.ThermWorking == TRUE){
            short i,data_1f4h,data_1f5h,thermvalue;                   
            write_register(0xfe,0x01);
            data_1f4h=read_register(0xf4);
            data_1f5h=read_register(0xf5);
            write_register(0xfe,0x00);
            thermvalue = (((data_1f4h&0x7f)<<8)|(data_1f5h&0xff))&0x7fff;
            
            if((thermvalue<0x2710)||(thermvalue>0x4e20)){
                return;
            }
            if((ThermComp.TxGainState&0xffff)==0xff30){
                ThermTxComp(thermvalue);
            }
            write_register(0x76,(data_1f4h&0x7f));
            write_register(0x77,data_1f5h);
            
        }
    }else if(0x04 == data_59h){
            short data_1f4h,data_1f5h;                   
            write_register(0xfe,0x01);
            data_1f4h=read_register(0xf4);
            data_1f5h=read_register(0xf5);
            write_register(0xfe,0x00);
            
            write_register(0x76,(data_1f4h&0x7f));
            write_register(0x77,data_1f5h);  
    }      
    response_bb(11,5,(0x40));
}
/*         
// 16
void thermometer_isr()
{

    clear_interrupt(0x05,0x08);

    therm_internal_trigger();
      
}*/
// warning, only called before xcpu&wcpu deep sleep
void gsm_therm_off_isr()
{
    write_register(0xfe,0x00); // GSM_Spi chaos debug
    response_bb(18,3,(0xa0));
    clear_interrupt(0x01,0x04);
    
    if(TRUE==ThermFsm.WPathWorking){
        return;
    }
    
    if((TRUE==ThermFsm.InnerState.StateOpen)&&(FALSE==ThermFsm.InnerState.StateCopy)){
        therm_value_update();    
    }
    
    therm_internal_close();
    ThermFsm.InnerState.StateOpen = FALSE;
    ThermFsm.InnerState.StateCopy = FALSE;
        
    response_bb(18,3,(0x40));
}
// warning, should called after initial_isr&calib_isr 
void gsm_therm_on_isr()
{
    write_register(0xfe,0x00); // GSM_Spi chaos debug
    response_bb(19,3,(0xa0));
    clear_interrupt(0x01,0x08);

    ThermFsm.InnerState.StateOpen = TRUE;            
    therm_internal_trigger();
    ThermFsm.InnerState.StateCopy = FALSE;

    response_bb(19,3,(0x40));
}
// therm update during flight mode when no rx_off&rf_off
void gsm_therm_update_isr()
{
    write_register(0xfe,0x00); // GSM_Spi chaos debug
    response_bb(17,3,(0xa0));
    clear_interrupt(0x01,0x02);

    if(TRUE==ThermFsm.InnerState.StateOpen){
        therm_value_update();
    }
    
    response_bb(17,3,(0x40));
}
