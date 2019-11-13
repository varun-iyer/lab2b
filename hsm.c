#include "lib/qpn_port.h"
#include "drw.h"
#include "bsp.h"
#include "hsm.h"

static QState hsm_init(hsm *mcn);
static QState hsm_on(hsm *mcn);
static QState hsm_active(hsm *mcn);
static QState hsm_inactive(hsm *mcn);
 
void hsm_ctor(void)  {
	QActive_ctor(&(machine.super_), (QStateHandler)& hsm_init);
	machine.disp_tmr = 0;
	machine.vol = 0;
}

QState hsm_init(hsm *mcn) {
	xil_printf("Initializing HSM\r\n");
    return Q_TRAN(&hsm_on);
}

QState hsm_on(hsm *mcn) {
	switch (Q_SIG(mcn)) {
		case Q_ENTRY_SIG:
			drw_clr(0, 0, LCD_WIDTH, LCD_HEIGHT);
		case Q_INIT_SIG:
			return Q_TRAN(&hsm_inactive);
		case TICK_SIG:
			return Q_HANDLED();
		case ENC_BTN_SIG:
			mcn->vol = 0;
			drw_vol(mcn->vol);
			return Q_TRAN(&hsm_active);
		case A_SIG:
			drw_txt("A");
			return Q_HANDLED();
		case B_SIG:
			drw_txt("B");
			return Q_HANDLED();
		case C_SIG:
			drw_txt("C");
			return Q_HANDLED();
		case D_SIG:
			drw_txt("D");
			return Q_HANDLED();
		case E_SIG:
			drw_txt("E");
			return Q_HANDLED();

	}
	return Q_SUPER(&QHsm_top);
}

QState hsm_active(hsm *mcn) {
	switch (Q_SIG(mcn)) {
		case Q_ENTRY_SIG:
			mcn->disp_tmr = INACTIVE_TICKS;
			return Q_HANDLED();
		case Q_EXIT_SIG:
			return Q_HANDLED();
		case LEFT_SIG:
			if(mcn->vol > VOL_INC) 
				mcn->vol -= VOL_INC;

			drw_vol(mcn->vol);
			return Q_HANDLED();
			break;
		case RIGHT_SIG:
			if(mcn->vol < 255 - VOL_INC) 
				mcn->vol += VOL_INC;

			drw_vol(mcn->vol);
			return Q_HANDLED();
			break;
		case TICK_SIG:
			tick_enc();
			mcn->disp_tmr--;
			if(mcn->disp_tmr == 0) {
				return Q_TRAN(&hsm_inactive);
			}
			else {
				return Q_HANDLED();
			}
	}
	return Q_SUPER(&hsm_on);
}

QState hsm_inactive(hsm *mcn) {
	switch (Q_SIG(mcn)) {
		case Q_ENTRY_SIG:
			clr_vol(mcn->vol);
			return Q_HANDLED();
		case Q_EXIT_SIG: 
			init_vol(mcn->vol);
			return Q_HANDLED();
		case TICK_SIG:
			tick_enc();
			return Q_HANDLED();
		case LEFT_SIG:
			drw_vol(mcn->vol);
			return Q_TRAN(&hsm_active);
		case RIGHT_SIG:
			drw_vol(mcn->vol);
			return Q_TRAN(&hsm_active);
	}
	return Q_SUPER(&hsm_on);
}
