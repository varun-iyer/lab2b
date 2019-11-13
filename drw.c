#include "lib/lcd.h"
#include "drw.h"
#define set_pix(x, y) fillRect(x, y, x, y)

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

void drw_txt(char* text) {
	setFont(BigFont);
	lcdPrint(text, LCD_WIDTH / 2 - 8, LCD_HEIGHT / 2 - 8);
}

void clr_txt(void) {
	drw_clr(LCD_WIDTH / 2 - 16, LCD_HEIGHT / 2 - 16, LCD_WIDTH / 2 + 16, LCD_HEIGHT / 2 + 16);
}

void drw_clr(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	u32 bg_color = 0x333333;
	setColor((bg_color >> 16) & 0xFF, (bg_color >> 8) & 0xFF, bg_color & 0xFF);
	fillRect(x1, y1, x2, y2);

	// Draw squares
	setColor(127, 127, 127);
	for(int y = 5; y + 30 < LCD_HEIGHT; y += 40) {
		for(int x = 5; x + 30 < LCD_WIDTH; x += 40) {
			if(((y + 30) > y1) && (y < y2) && (x < x2) && ((x + 30) > x1)) {
				fillRect(max(x, x1), y, min(x + 30, x2), y + 30);
			}
		}
	}
}

static uint8_t prev_vol = 0;
#define vol2pix(vol) (vol * (DELTA_VOL) / 0xFF + VOL_X1)

void clr_vol(uint8_t vol) {
	drw_clr(VOL_X1, VOL_Y1, vol2pix(vol), VOL_Y2);
}

void init_vol(uint8_t vol) {
	prev_vol = 0;
	drw_vol(vol);
}

void drw_vol(uint8_t vol) {
	if(vol == prev_vol) return;
	setColor((VOL_COL >> 16) & 0xFF, (VOL_COL >> 8) & 0xFF, VOL_COL & 0xFF);
	if(vol > prev_vol) {
		fillRect(vol2pix(prev_vol), VOL_Y1, vol2pix(vol), VOL_Y2);
	}
	else {
		drw_clr(vol2pix(vol), VOL_Y1, vol2pix(prev_vol), VOL_Y2);
	}
	prev_vol = vol;
}
