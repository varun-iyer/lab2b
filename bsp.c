#include "lib/qpn_port.h"
#include "bsp.h"
//#include "lab2a.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xtmrctr.h"
#include "hsm.h"
#include "xil_cache.h"
#include "xspi.h"
#include "lib/lcd.h"


#define GPIO_CHANNEL1 1

XGpio enc, lcd;
XTmrCtr ctr;
XIntc intc;

#define chk_status(...) do { \
		if (status != XST_SUCCESS) { \
			xil_printf(__VA_ARGS__); \
			return XST_FAILURE; \
		} \
	} while(0)

void tmr_handler(void *CallBackRef, u8 TmrCtrNumber) {
	QActive_postISR((QActive *) &machine, TICK_SIG);
	XTmrCtr_WriteReg(ctr.BaseAddress, 0, XTC_TCSR_OFFSET, XTmrCtr_ReadReg(ctr.BaseAddress, 0, XTC_TCSR_OFFSET | XTC_CSR_INT_OCCURED_MASK));
}

uint8_t curstate;

enum enc_states {
	CW_T1, CCW_T1, CW_T2, CCW_T2, REST
};

void enc_handler(void *baseaddr_p) {
	Xuint32 dsr = XGpio_DiscreteRead(&enc, 1);
	XGpio_InterruptClear(&enc, 1);
	switch(dsr & 0b11) {
		case 0b11:
			if(curstate == CW_T2) QActive_postISR((QActive *) &machine, LEFT_SIG);
			if(curstate == CCW_T2) QActive_postISR((QActive *) &machine, RIGHT_SIG);
			curstate = REST;
			break;
		case 0b01:
			curstate = (curstate == REST) ? CCW_T1 : curstate;
			break;
		case 0b10:
			curstate = (curstate == REST) ? CW_T1 : curstate;
			break;
		case 0b00:
			curstate = (curstate == CW_T1) ? CW_T2 : ((curstate == CCW_T1) ? CCW_T2 : curstate);
			break;
	}
}

//static XIntc intc;
static XTmrCtr axiTimer;
static XGpio lcd_gpio;
static XSpi lcd_spi;

void BSP_init(void) {
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	u32 status;

	// Initialize TmrCtr
	status = XTmrCtr_Initialize(&axiTimer, XPAR_AXI_TIMER_0_DEVICE_ID);
	chk_status("Failed to initialize TmrCtr!");

	// Initialize intc
	status = XIntc_Initialize(&intc, XPAR_INTC_0_DEVICE_ID);
	chk_status("Failed to initialize Intc!\n");
	status = XIntc_Connect(&intc,
				XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR,
				(XInterruptHandler)XTmrCtr_InterruptHandler,
				(void *)&axiTimer);
	chk_status("Failed to connect Intc!\n");
	status = XIntc_Start(&intc, XIN_REAL_MODE);
	chk_status("Failed to start Intc!\n");

	// Configure TmrCtr
	XTmrCtr_SetHandler(&axiTimer, tmr_handler, &axiTimer);
	XTmrCtr_SetOptions(&axiTimer, 0,
				XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetResetValue(&axiTimer, 0, 0xF0000000); // CHANGE Timer Period here
	XTmrCtr_Start(&axiTimer, 0);
	xil_printf("Started timer!\n");
	 
	// Initialize GPIO 
	status = XGpio_Initialize(&lcd_gpio, XPAR_SPI_DC_DEVICE_ID);
	chk_status("Failed to initialize GPIO!\n");
	XGpio_SetDataDirection(&lcd_gpio, 1, 0x0);

	// Intialize SPI
	u32 controlReg;
	XSpi_Config *spiConfig;	/* Pointer to Configuration data */
	spiConfig = XSpi_LookupConfig(XPAR_SPI_0_DEVICE_ID);
	status = XSpi_CfgInitialize(&lcd_spi, spiConfig, spiConfig->BaseAddress);
	chk_status("Cannot find SPI Device!");
	XSpi_Reset(&lcd_spi);
	controlReg = XSpi_GetControlReg(&lcd_spi);
	XSpi_SetControlReg(&lcd_spi,
			(controlReg | XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK) &
			(~XSP_CR_TRANS_INHIBIT_MASK));
	XSpi_SetSlaveSelectReg(&lcd_spi, ~0x01);

	// Init lcd
	initLCD();
	//initialize encoder
	XGpio_Initialize(&enc, XPAR_AXI_GPIO_ROTARY_DEVICE_ID); // init gpio
	XGpio_SetDataDirection(&enc, 1, 0x01); // input port
	XGpio_InterruptEnable(&enc, XGPIO_IR_CH1_MASK); // input port
	XGpio_InterruptGlobalEnable(&enc);
	XGpio_SetDataDirection(&enc, 1, 0x01); // input port
	XIntc_Initialize(&intc, XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);
	XIntc_Connect(&intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ROTARY_IP2INTC_IRPT_INTR, (XInterruptHandler) enc_handler, &enc);
	curstate = 0b11;
}

/*..........................................................................*/
void QF_onStartup(void) {                 /* entered with interrupts locked */
	XIntc_Enable(&intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_ROTARY_IP2INTC_IRPT_INTR);
	XIntc_Enable(&intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
	microblaze_enable_interrupts();
}


void QF_onIdle(void) {        /* entered with interrupts locked */
    QF_INT_UNLOCK();                       /* unlock interrupts */
}

/* Do not touch Q_onAssert */
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_LOCK();
    for (;;) {
    }
}
