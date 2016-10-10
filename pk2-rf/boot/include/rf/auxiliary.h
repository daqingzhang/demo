//-------------------------------
// register releated stuff
//-------------------------------

#ifndef AUXILIARY_H
#define AUXILIARY_H

#include "register.h"
#include "calib_wcdma.h"

/* Global macro definitions  */

#define TRUE    1
#define FALSE   0

/* Therm variables and struct definitions */
typedef struct{
    char    StateOpen;
    char    StateCopy;
}Therm_In_T;//Internal interrupt state

typedef struct{
    char    ThermWorking;
    char    ThermUseable;
    char    PwdWorking;
    char    PwdUseable;
    char    WPathWorking;
    char    GPathWorking;
    Therm_In_T      InnerState;
}Therm_State_T;

typedef struct{
    short   TxGainState;
    short   TxCalibTemp;
    short   TxGainComp[11];
}Therm_Calib_T;

typedef struct{
    short   TxTempState;
    short   TxTempReset[11];
    short   TxGainOffset;
    short   TxCurrTemper;
}Therm_TxTemp_T;

/*  Therm functions declaration  */

extern void therm_state_init(void) property(loop_levels_0);
extern void therm_internal_trigger(void) property(loop_levels_0);
extern void therm_value_update(void) property(loop_levels_0);
extern void thermometer_cycle(void) property(loop_levels_0);
extern void mipi_rffe_register_write(char sa, char address, char data) property(loop_levels_1);

/* Therm variables declaration  */

extern Therm_TxTemp_T ThermTemp;
extern Therm_State_T ThermFsm;
extern Therm_Calib_T ThermComp;
extern short therm_apc_ana_offset[CALIB_WCDMA_TX_ANA_GAIN_NUM];
extern short therm_tempe[11];
extern short last_rf_state;

#endif