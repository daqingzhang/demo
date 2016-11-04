#include <regs_general.h>

#define ADDR_BASE  0x00000000

REG_DEF_S dfe_regs[] =
{
/*G_DEF_S dfe_reg_mode 				*/ INIT_REG_DEF_S( ADDR_BASE + 0x32000,	  0x60,	  0x7f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_clk_mode 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32004,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_dcc 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32008,	   0x0,	  0x7f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_dc_calib_re 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3200c,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_dc_calib_im 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32010,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_dc_delta_re 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32014,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_dc_delta_im 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32018,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_dc_cr 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x3201c,	   0x0,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_slow_bw_ct 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32020,	   0x0,	   0x7,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_gain_ct_reg 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32024,	   0x0,	 0x1ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_imbc_ct 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32028,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_imbc_tmr_bw_idle 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x3202c,	   0x0,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_imbc_tmr_bw_fast 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32030,	   0x0,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_imbc_tmr_bw_slow 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32034,	   0x0,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_imbc_thd_wa_delta	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32038,	   0x0,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_imbc_thd_wq_delta	*/ INIT_REG_DEF_S( ADDR_BASE + 0x3203c,	   0x0,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_imbc_bw_fast_ct 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32040,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_imbc_bw_slow_ct 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32044,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_imbc_wa 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32048,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_imbc_wq 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x3204c,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_rc_strech 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32050,	   0xc,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_rc_rate_ofs_period 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32054,	  0x10,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_rc_rate_ofs_hi 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32058,	   0x8,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_rc_rate_ofs_lo 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3205c,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_rc_rate_ofs_rest 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32060,	   0x0,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_gdeq_coef0_rg_1 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32064,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_gdeq_coef0_rg_2		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32068,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_gdeq_coef1_rg_1		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3206c,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_gdeq_coef1_rg_2		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32070,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_gdeq_coef2_rg_1		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32074,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_gdeq_coef2_rg_2		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32078,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_gdeq_coef3_rg_1		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3207c,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_gdeq_coef3_rg_2		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32080,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_gdeq_bypass		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32084,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rxdp_imbc_calc_rels		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32088,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_a1			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32100,	   0x0,	 0xfff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_a2			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32104,	   0x0,	 0xfff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_b1			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32108,	   0x0,	 0xfff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_b2			*/ INIT_REG_DEF_S( ADDR_BASE + 0x3210c,	   0x0,	 0xfff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_g			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32110,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_equ_bypass_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32114,	   0x1,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_equ_tx_shift_ct 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32118,	   0x0,	  0x3f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_offset_value0_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x3211c,	0xaaab,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_offset_value1_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32120,	   0xa,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_tx_rx		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32124,	   0x1,	   0x3,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_freq0		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32128,	0x6270,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_freq1		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3212c,	0x9227,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_freq_tx_offset0	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32130,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_freq_tx_offset1	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32134,	   0x0,	   0x3,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_sdmpre_ct		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32138,	  0x10,	  0x7f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_pn_en_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3213c,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_pn_switch_reg	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32140,	   0x0,	   0x7,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_gsm_encode_en_reg	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32144,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_gain_ct_reg	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32200,	   0x0,	 0x1ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_ct 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32204,	   0x0,	  0x7f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_i_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32208,	   0x0,	 0xfff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_q_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3220c,	   0x0,	 0xfff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_pherr_reg	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32210,	   0x0,	 0xfff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_amerr_reg	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32214,	   0x0,	 0xfff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_gain1_ct	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32218,	   0x0,	  0x7f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_gain2_ct_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x3221c,	   0x0,	  0x7f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_lpf_ct_rg_reg */ INIT_REG_DEF_S( ADDR_BASE + 0x32220,	   0x0,	   0x7,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_fdb_ct_rg_reg */ INIT_REG_DEF_S( ADDR_BASE + 0x32224,	   0x0,	   0x7,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_filter_ct_rg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32228,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_bit_ct_rg_reg */ INIT_REG_DEF_S( ADDR_BASE + 0x3222c,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_phbit_ct	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32230,	   0x0,	   0x7,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_ambit_ct	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32234,	   0x0,	   0x7,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_loft_tpm_dac 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32238,	   0x0,	  0x1f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_rc_stretch_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x3223c,	   0x0,	  0x3f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_rc_rate_ofs_period_rg_reg*/ INIT_REG_DEF_S( ADDR_BASE + 0x32240,	   0x0,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_rc_rate_ofs_hi_rg_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32244,	  0x78,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_rc_rate_ofs_lo_rg_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32248,	0x7878,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_rc_rate_ofs_rest_rc_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x3224c,	   0x8,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_atpg_mode_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32250,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_comp_bypass_reg */ INIT_REG_DEF_S( ADDR_BASE + 0x32254,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_comp_bypass_reg */ INIT_REG_DEF_S( ADDR_BASE + 0x32258,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_ampm_comp_bp_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x3225c,	   0x1,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_shift_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32260,	   0x0,	   0x7,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p0_reg 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32264,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p1_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32268,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p2_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3226c,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p3_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32270,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p4_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32274,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p5_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32278,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p6_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3227c,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p7_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32280,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p8_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32284,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p9_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32288,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p10_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3228c,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p11_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32290,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p12_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32294,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p13_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32298,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p14_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3229c,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p15_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322a0,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_am_p16_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322a4,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p0_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322a8,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p1_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322ac,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p2_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322b0,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p3_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322b4,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p4_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322b8,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p5_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322bc,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p6_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322c0,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p7_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322c4,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p8_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322c8,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p9_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322cc,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p10_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322d0,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p11_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322d4,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p12_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322d8,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p13_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322dc,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p14_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322e0,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p15_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322e4,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_pm_p16_reg		*/ INIT_REG_DEF_S( ADDR_BASE + 0x322e8,	   0x0,	 0x3ff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_tx_gain_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x322ec,	   0x0,	  0xff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_ramp_bypass_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x322f0,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_iq_swap_tx_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x322f4,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_dly_ct_amp1_reg	*/ INIT_REG_DEF_S( ADDR_BASE + 0x322f8,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_dly_ct_amp2_reg	*/ INIT_REG_DEF_S( ADDR_BASE + 0x322fc,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_dly_ct_amp3_reg	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32300,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_dly_ct_amp4_reg	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32304,	   0x0,	   0xf,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_amp_ref_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x32308,	   0x0,	 0xfff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_txdp_wedge_amp_limit_bp_reg 	*/ INIT_REG_DEF_S( ADDR_BASE + 0x3230c,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_sincos_iqswap 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x323f4,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_sincos_fre 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x323f8,	   0xf,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_sincos_amp 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x323fc,	0x3fff,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_adc_bypass_reg 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32400,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_tsct 				*/ INIT_REG_DEF_S( ADDR_BASE + 0x32404,	  0x10,	  0x1f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_sincos_ct_tmp 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32408,	   0x0,	   0x7,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_tx_en 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x3240c,	   0x0,	  0x1f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_num_gmsk_dly 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32410,	   0x0,	  0x3f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_num_edge_dly			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32414,	   0x0,	  0x1f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_num_wd_dly			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32418,	   0x0,	  0x1f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_wr_clr_val			*/ INIT_REG_DEF_S( ADDR_BASE + 0x3241c,	   0x0,	  0x3f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rd_clr_val			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32420,	   0x0,	  0x3f,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rssi_shift			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32424,	   0x0,	   0x7,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rssi_acc_len			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32428,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rssi_start 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x3242c,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rssi_o 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32430,	   0x0,	   0x0,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_rssi_calc_done 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32434,	   0x0,	   0x0,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_pwr_rf_ushift 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32438,	   0x0,	   0x7,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_pwr_rf_acc_len 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32440,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_pwr_rf_start 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32454,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_pwr_rf_polar 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32458,	   0x1,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_pwr_bb_ushift_reg 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3245c,	   0x0,	   0x7,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_pwr_bb_acc_len_reg 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32460,	   0x0,	0xffff,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_pwr_bb_start_reg 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32464,	   0x0,	   0x1,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_pwr_rf_o_rg 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32468,	 0x160,	   0x0,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_pwr_rf_calc_done_reg 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x3246c,	   0x0,	   0x0,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_pwr_bb_o_reg 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32470,	 0x160,	   0x0,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_pwr_bb_calc_done_reg 		*/ INIT_REG_DEF_S( ADDR_BASE + 0x32474,	   0x0,	   0x0,  1,	0xffff, 0x1),
/*G_DEF_S dfe_reg_fifo_clr_ct 			*/ INIT_REG_DEF_S( ADDR_BASE + 0x32478,	   0x0,	   0xf,  1,	0xffff, 0x1),
};

int dfe_reg_hw_reset_test(void)
{
	int i,err = 0;
	REG_DEF_S *ptr_regs = dfe_regs;

	for(i = 0; i < ARRAY_SIZE(dfe_regs); i++) {
		if(reg16_hw_reset_test(ptr_regs) != 0) {
			PRINTF("dfe_reg_hw_reset_test, error addr = ", ptr_regs->address);
			//return -1;
			err++;
		}
		ptr_regs++;
	}
	if(!err)
		serial_puts("dfe_reg_hw_reset_test, test success !\n");
	return 0;
}

int dfe_reg_rw_test(void)
{
	int i,err = 0;
	REG_DEF_S *ptr_regs = dfe_regs;

	for(i = 0; i < ARRAY_SIZE(dfe_regs); i++) {
		if(reg16_rw_test(ptr_regs) != 0) {
			PRINTF("dfe_reg_rw_test, error addr = ", ptr_regs->address);
			//return -1;
			err++;
		}
		ptr_regs++;
	}
	if(!err)
		serial_puts("dfe_reg_rw_test, test success !\n");
	return 0;
}

int dfe_reg_test(void)
{
	int r; 

	r = dfe_reg_hw_reset_test();
	r += dfe_reg_rw_test();

	return r;
}
