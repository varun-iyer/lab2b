/*****************************************************************************
* main.c for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#include "lib/qpn_port.h"                                       /* QP-nano port */
#include "bsp.h"                             /* Board Support Package (BSP) */
#include "hsm.h"
//#include "lab2a.h"                               /* application interface */



static QEvent hsmqueue[30];  

QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
	{ (QActive *)0,            (QEvent *)0,          0                    },
	{ (QActive *)&machine,    hsmqueue,         Q_DIM(hsmqueue)  }
};

Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);

// Do not edit main, unless you have a really good reason
int main(void) {
	hsm_ctor(); // inside of lab2a.c
	BSP_init(); // inside of bsp.c, starts out empty!
	QF_run(); // inside of qfn.c
	return 0;
}
