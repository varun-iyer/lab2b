#include "lib/qpn_port.h"
#include "drw.h"
#include "bsp.h"
#include "hsm.h"

enum hsm_sig {
	LEFT_SIG = Q_USER_SIG,
	RIGHT_SIG,
	TICK_SIG
};
 
struct hsm  {               //Lab2A State machine
	QActive super_; // inheritance from QP
	uint32_t disp_tmr; // display timer
	uint8_t vol;
};

struct hsm machine;

static QState hsm_on(struct hsm *mcn);
static QState hsm_active(struct hsm *mcn);
static QState hsm_inactive(struct hsm *mcn);
 
void hsm_ctor(void)  {
	QActive_ctor(&(machine.super_), (QStateHandler) &Lab2A_initial);
	machine.disp_tmr = 0;
	machine.vol = 0;
}

QState hsm_init(struct hsm *mcn) {
	xil_printf("Initializing HSM\r\n");
    return Q_TRAN(&hsm_top);
}

QState hsm_on(struct hsm *mcn) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG:
		case Q_INIT_SIG:
			return Q_TRAN(&hsm_inactive);
		case LEFT_SIG:
			mcn->vol--;
			return Q_TRAN(&hsm_active);
			break;
		case RIGHT_SIG:
			mcn->vol++;
			return Q_TRAN(&hsm_active);
			break;
	}
	return Q_SUPER(&QHsm_top);
}

QState hsm_active(struct hsm *mcn) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG:
			mcn->disp_tmr = INACTIVE_TICKS;
		case Q_EXIT_SIG:
			return Q_HANDLED();
		case TICK_SIG:
			drw_bkg();
			drw_vol(mcn->vol);
			mcn->disp_tmr--;
			if(disp_tmr == 0) {
				return Q_TRAN(&hsm_inactive);
			}
			else {
				return Q_HANDLED();
			}
	}
	return Q_SUPER(&hsm_on);
}

QState hsm_inactive(struct hsm *mcn) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG:
		case Q_EXIT_SIG: return Q_HANDLED();
		case TICK_SIG:
			drw_bkg();
			return Q_HANDLED();
	}
	return Q_SUPER(&hsm_on);
}
