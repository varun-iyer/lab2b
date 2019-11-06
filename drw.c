#include "lib/lcd.h"
#define set_pix(x, y) fillRect(x, y, x, y)

void drw_clear(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	u32 bg_color = 0x333333;
	setColor((bg_color >> 16) & 0xFF, (bg_color >> 8) & 0xFF, bg_color & 0xFF);
	fillRect(x1, y1, x2, y2);
	// Draw squares
	setColor(127, 127, 127);
	for(int y = 5; y + 30 < LCD_HEIGHT; y += 40) {
		for(int x = 5; x + 30 < LCD_WIDTH; x += 40) {
			if(((y + 30) > y1) && (y < y2) && (x < x2) && ((x + 30) > x1)) {
				fillRect(x, y, x + 30, y + 30);
			}
		}
	}
}

void drw_vol(uint8_t vol) {
	uint16_t mid =  VOL_X1 + ((VOL_X2 - VOL_X1) * 0xFF) / vol;
	drw_clr(mid + 1, VOL_Y1, VOL_X2, VOL_Y2);
	setColor((VOL_COL >> 16) & 0xFF, (VOL_COL >> 8) & 0xFF, VOL_COL & 0xFF);
	fillRect(VOL_X1, VOL_Y1, mid, VOL_Y2);
}
