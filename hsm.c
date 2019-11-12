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
		case Q_INIT_SIG:
			return Q_TRAN(&hsm_inactive);
		case TICK_SIG:
		case LEFT_SIG:
			drw_clr(0, 0, LCD_WIDTH, LCD_HEIGHT);
			drw_vol(mcn->vol);

			if(mcn->vol > 0) mcn->vol--;
			return Q_TRAN(&hsm_active);
			break;
		case RIGHT_SIG:
			drw_vol(mcn->vol);

			if(mcn->vol < 255) mcn->vol++;
			return Q_TRAN(&hsm_active);
			break;

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
		case TICK_SIG:
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
			drw_clr(0, 0, LCD_WIDTH, LCD_HEIGHT);
			return Q_HANDLED();
		case Q_EXIT_SIG: return Q_HANDLED();
		case TICK_SIG:
			return Q_HANDLED();
	}
	return Q_SUPER(&hsm_on);
}
