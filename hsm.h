#ifndef hsm_h
#define hsm_h
void hsm_ctor(void);
// 2 seconds  * ticks per second
#define INACTIVE_TICKS (2 * 5)


enum hsm_sig {
	LEFT_SIG = Q_USER_SIG,
	RIGHT_SIG,
	TICK_SIG,
	ENC_BTN_SIG,
	A_SIG,
	B_SIG,
	C_SIG,
	D_SIG,
	E_SIG
};

typedef struct hsmTag {               //Lab2A State machine
	QActive super_; // inheritance from QP
	uint32_t disp_tmr; // display timer
	uint8_t vol;
} hsm;

hsm machine;
#endif  
