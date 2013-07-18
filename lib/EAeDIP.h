/*
	library for communicating with EA DIP LCD screen
*/

extern void edip_init();

struct EDIP {
	int (*puts)(char *, int);
	int (*pending)();
	unsigned char (*getc)();
};

enum EDIP {
	TOP = 0, MIDDLE = 3, BOTTOM = 6,
	LEFT = 1, CENTER = 2, RIGHT = 3,
/* color table:
	000000	black
	0000FF	blue
	FF0000	red
	00FF00	green
	FF00FF	magenta
	00FFFF	cyan
	FFFFFF	white
	6F6F6F	gray
	FF8F00	ornage
	8F00FF	purple
	FF008F	pink
	00FF8F	light green
	8FFF00	light red
	008FFF	light blue
	AFAFAF	silver
*/
	TRANSPARENT = 0, BLACK = 1,
	BLUE = 2, RED = 3, GREEN = 4,
	MAGENTA = 5, CYAN = 6, YELLOW = 7,
	WHITE = 8, GRAY = 9,
	ORANGE = 10, PURPLE = 11, PINK = 12,
	LIGHT_GREEN = 13, LIGHT_RED = 14, LIGHT_BLUE = 15,
	SILVER = 16
};

// clear the screen
void edip_clear(const struct EDIP *);
// select a font
void edip_font(const struct EDIP *, unsigned char font);
// font_color(lcd, foreground, background);
void edip_font_color(const struct EDIP *, unsigned char, unsigned char);
// font_zoom(lcd, x, y)
void edip_font_zoom(const struct EDIP *, unsigned char, unsigned char);
// text(lcd, x1, y1, x2, y2)
void edip_draw(const struct EDIP *, unsigned int, unsigned int, unsigned int, unsigned int);
// text(lcd, x1, y1, x2, y2, color)
void edip_fill(const struct EDIP *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned char);
// text(lcd, x1, y1, x2, y2, position, string)
void edip_text(const struct EDIP *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned char, char *);
// remove touch keys
void edip_clear_keys(const struct EDIP *);
// switch(lcd, x1, y1, x2, y2, down, up, string)
void edip_switch(const struct EDIP *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned char, unsigned char, char *);
// button(lcd, x1, y1, x2, y2, down, up, string)
void edip_button(const struct EDIP *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned char, unsigned char, char *);
