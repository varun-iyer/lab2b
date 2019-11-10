#include <stdio.h>		// Used for printf()
#include "xparameters.h"	// Contains hardware addresses and bit masks
#include "xil_cache.h"		// Cache Drivers
#include "xintc.h"		// Interrupt Drivers
#include "xtmrctr.h"		// Timer Drivers
#include "xtmrctr_l.h" 		// Low-level timer drivers
#include "xil_printf.h" 	// Used for xil_printf()
#include "xgpio.h" 		// LED driver, used for General purpose I/i
#include "xspi.h"
#include "xspi_l.h"
#include "lib/lcd.h"

volatile int timerTrigger = 0;
 
static XIntc intc;
static XTmrCtr tmr;
static XTmrCtr axiTimer;
static XGpio lcd_gpio;
static XSpi lcd_spi;


void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber)
{
	timerTrigger = 100;
}
 
 
#define chk_status(...) do { \
		if (status != XST_SUCCESS) { \
			xil_printf(__VA_ARGS__); \
			return XST_FAILURE; \
		} \
	} while(0)
 
u32 setup() {
	u32 status;

	// Initialize TmrCtr
	status = XTmrCtr_Initialize(&axiTimer, XPAR_AXI_TIMER_0_DEVICE_ID);
	chk_status("Failed to initialize TmrCtr!");
	XTmrCtr_SetHandler(&axiTimer, TimerCounterHandler, &axiTimer);
	XTmrCtr_SetOptions(&axiTimer, 0,
				XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetResetValue(&axiTimer, 0, 0xFFFF0000); // CHANGE Timer Period here
	XTmrCtr_Start(&axiTimer, 0);
	xil_printf("Started timer!\n");

	// Initialize intc
	status = XIntc_Connect(&intc,
				XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR,
				(XInterruptHandler)XTmrCtr_InterruptHandler,
				(void *)&axiTimer);
	chk_status("Failed to connect Intc!\n");
	status = XIntc_Initialize(&intc, XPAR_INTC_0_DEVICE_ID);
	chk_status("Failed to initialize Intc!\n");
	status = XIntc_Start(&intc, XIN_REAL_MODE);
	chk_status("Failed to start Intc!\n");
	XIntc_Enable(&intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
	microblaze_enable_interrupts();
	 
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
	 
	Xil_ICacheEnable();
	Xil_DCacheEnable();
}


int main()
{

	int sec = 0;
	int secTmp;
	char secStr[4];

	setup();
	 
	clrScr();

	setColor(255, 0, 0);
	fillRect(20, 20, 220, 300);

	setColor(0, 50, 100);
	fillRect(40, 180, 200, 250);

	setColor(0, 255, 0);
	setColorBg(255, 0, 0);
	lcdPrint("Hello !!!!!", 40, 60);

	setFont(BigFont);
	lcdPrint("<# WORLD #>", 40, 80);

	setFont(SevenSegNumFont);
	setColor(238, 64, 0);
	setColorBg(0, 50, 100);
	while (1) {
		if (timerTrigger > 0) {
			secTmp = sec++ % 1000;
			secStr[0] = secTmp / 100 + 48;
			secTmp -= (secStr[0] - 48) * 100;
			secStr[1] = secTmp / 10 + 48;
			secTmp -= (secStr[1] - 48) * 10;
			secStr[2] = secTmp + 48;
			secStr[3] = '\0';

			lcdPrint(secStr, 70, 190);

			timerTrigger = 0;
		}
	}

	xil_printf("End\n");
	return 0;
}
