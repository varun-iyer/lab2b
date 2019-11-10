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

	return status;
}

 
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define setPix(x, y) fillRect(x, y, x, y)
void draw_background () {
	u32 pixs = 0x333333;
	for(int x = 0; x < LCD_WIDTH; x++) {
		for(int y = 0; y < LCD_HEIGHT; y++) {
			setColor((pixs >> 16) & 0xFF, (pixs >> 8) & 0xFF, pixs & 0xFF);
			setPix(x, y);
		}
	}

	setColor(255, 255, 255);
	for(int y = 10; y + 30 < LCD_WIDTH; y += 40) {
		for(int x = 10; x + 30 < LCD_WIDTH; x += 40) {
			fillRect(x, y, x + 30, y + 30);
		}
	}
}

 
int main()
{
	int sec = 0;
	int secTmp;
	char secStr[4];

	setup();
	 
	clrScr();

	draw_background();
	return 0;
}
