#define LCD_WIDTH 240
#define LCD_HEIGHT 320
void drw_clr(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
#define drw_bkg drw_clr(0, 0, LCD_WIDTH, LCD_HEIGHT);
void drw_vol(uint8_t vol);
#define drw_vol_clr() drw_vol(0)
void clr_vol(uint8_t vol);
void clr_txt();
void init_vol(uint8_t vol);
void drw_txt(char* text);

#define VOL_X1 10
#define VOL_X2 230
#define DELTA_VOL VOL_X2 - VOL_X1
#define VOL_Y1 10
#define VOL_Y2 60
#define VOL_COL 0xEEEEEE
#define VOL_INC 220/64	// Volume bar increment
