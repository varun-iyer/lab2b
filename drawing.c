#include "lib/lcd.h"

// TODO This file needs a better name

#define LCD_WIDTH 240
#define LCD_HEIGHT 320
#define setPix(x, y) fillRect(x, y, x, y)
void draw_background () {
	// Draw background
	u32 bg_color = 0x333333;
	setColor((bg_color >> 16) & 0xFF, (bg_color >> 8) & 0xFF, bg_color & 0xFF);
	fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT);

	// Draw squares
	setColor(127, 127, 127);
	for(int y = 5; y + 30 < LCD_HEIGHT; y += 40) {
		for(int x = 5; x + 30 < LCD_WIDTH; x += 40) {
			fillRect(x, y, x + 30, y + 30);
		}
	}
}

