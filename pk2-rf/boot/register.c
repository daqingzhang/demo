extern long long time_trace;
extern short last_rf_state;

volatile signed int *WADDR_EXT_REG = (signed int *)0x3fff0;
volatile signed int *RADDR_EXT_REG = (signed int *)0x3ffe0;

unsigned int data_read;
unsigned int response_bb_counter=1;
unsigned short w_hw_int_count[4]={0,0,0,0};//0x50,0x52,0x54,0x56

struct {
	int	IntLogBuff[50];// mem latest 50 int from spi
	int	IntEveBuff[32];// mem latest state of every int
	short	BIndex;// record num count of 50
	short	EAccum;// error num count
	short	IntState;// int open or close
	short	IntNum;// int num
}LogRecord = {{0,},{0,},-1,0,0,0};

int read_register(const int addr){
	*WADDR_EXT_REG = 0xff000000|((addr<<8)&0x0000ff00);
	data_read = (*RADDR_EXT_REG>>8)&0x000000ff;
	return(data_read);
}

void write_register(const int addr,const int data){
	*WADDR_EXT_REG = 0xff000000|((addr<<8)&0x0000ff00)|((data)&(0xff));//(data&0x000000ff);
	*WADDR_EXT_REG = 0xdf000000|((addr<<8)&0x0000ff00)|((data)&(0xff));
}

void hw_int_accum(const short index){
  
	switch (index){
		case 15:
			if(0xff==w_hw_int_count[0])
				w_hw_int_count[0]=0;
			write_register(0xcc,++w_hw_int_count[0]);//wcdma_rx_pll
			break;
		case 14:
			if(0xff==w_hw_int_count[1])
				w_hw_int_count[1]=0;		
			write_register(0xcd,++w_hw_int_count[1]);//wcdma_agc
			break;
		case 13:
			if(0xff==w_hw_int_count[2])
				w_hw_int_count[2]=0;		
			write_register(0xce,++w_hw_int_count[2]);//wcdma_tx_pll
			break;   
		case 12:
			if(0xff==w_hw_int_count[3])
				w_hw_int_count[3]=0;		
			write_register(0xcf,++w_hw_int_count[3]);//wcdma_apc
			break; 
	}
}

void response_bb(const int index,const int code,const int location){
	int reply_data = 0;
	reply_data = (((index&0x1f)<<3)|(code&0x07));
	write_register(0xc8,reply_data);
	write_register(0xc9,(location|last_rf_state));
	
	if(0xa0==location){
		if(1==LogRecord.IntState){
			LogRecord.EAccum++;
		}
		LogRecord.BIndex++;
		LogRecord.BIndex=(LogRecord.BIndex>=50)?0:LogRecord.BIndex;
		LogRecord.IntLogBuff[LogRecord.BIndex]=((time_trace<<8)|((location|(index&0x1f))&0xff))&0xffffffff;
		LogRecord.IntEveBuff[index]=((time_trace<<8)|((location|(index&0x1f))&0xff))&0xffffffff;
		LogRecord.IntState=1;
	}else if(0x40==location){
		if(0==LogRecord.IntState){
			LogRecord.EAccum++;
			LogRecord.BIndex++;
			LogRecord.BIndex=(LogRecord.BIndex>=50)?0:LogRecord.BIndex;
			LogRecord.IntLogBuff[LogRecord.BIndex]=((time_trace<<8)|((location|(index&0x1f))&0xff))&0xffffffff;
			LogRecord.IntEveBuff[index]=((time_trace<<8)|((location|(index&0x1f))&0xff))&0xffffffff;
		}else{
			LogRecord.IntLogBuff[LogRecord.BIndex]|=location;
			LogRecord.IntEveBuff[index]|=location;
		}
		LogRecord.IntState=0;
	}
}

void clear_interrupt(const int addr,const int value){
	write_register((addr&0xff),(value&0xff));
	write_register((addr&0xff),0x00);
}	
