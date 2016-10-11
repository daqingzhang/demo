#ifndef __CALIB_WCDMA_H__
#define __CALIB_WCDMA_H__

#define CALIB_WCDMA_RX_ANA_GAIN_NUM  8	//7
#define CALIB_WCDMA_TX_ANA_GAIN_NUM  11	//13
#define CALIB_WCDMA_AGC_LNA_GAIN_SWITCH  5	//4
#define CALIB_WCDMA_TX_DC_DIG_GAIN_OFFSET 48	//Analogue gain 0 (<83)

#define H_BAND	1
#define L_BAND	0
#define IPATH	1
#define QPATH	0
#define ADD	1
#define SUB	0

extern char wcdma_agc_calib_ana_hb[CALIB_WCDMA_RX_ANA_GAIN_NUM][2];
extern char wcdma_agc_calib_dig_hb[CALIB_WCDMA_RX_ANA_GAIN_NUM][4];
extern char wcdma_agc_calib_ana_lb[CALIB_WCDMA_RX_ANA_GAIN_NUM][2];
extern char wcdma_agc_calib_dig_lb[CALIB_WCDMA_RX_ANA_GAIN_NUM][4];

extern short wcdma_apc_calib_ana_hb[CALIB_WCDMA_TX_ANA_GAIN_NUM][2];
extern short wcdma_apc_calib_ana_lb[CALIB_WCDMA_TX_ANA_GAIN_NUM][2];

#endif
