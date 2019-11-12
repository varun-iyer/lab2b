#include "lib/lcd.h"
#include "drw.h"
#define set_pix(x, y) fillRect(x, y, x, y)


void drw_clr(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	u32 bg_color = 0x333333;
	setColor((bg_color >> 16) & 0xFF, (bg_color >> 8) & 0xFF, bg_color & 0xFF);
	fillRect(x1, y1, x2, y2);

	// TODO Commented out to test efficiency
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

static uint8_tprev_vol = 0;
#define vol2pix(vol) (VOL_X1 + (((VOL_X2 - VOL_X1) * 0xFF) / vol))
void drw_vol(uint8_t vol) {
	if(vol == prev_vol) return;
	setColor((VOL_COL >> 16) & 0xFF, (VOL_COL >> 8) & 0xFF, VOL_COL & 0xFF);
	if(vol > prev_vol) {
		fillRect(vol2pix(prev_vol), VOL_Y1, vol2pix(vol), VOL_Y2);
	}
	else {
		drw_clr(vol2pix(prev_vol), VOL_Y1, vol2pix(vol), VOL_Y2);
	}
	prev_vol = vol;
}
