#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
//#include <std/string.h>

#include <tty.h>
#include <env.h>
#include <command.h>

#include <cpu/io.h>

#include <gfx/vga.h>
#include <gfx/vesa.h>
#include <gfx/surface.h>

#include <devices/keyboard.h>

static const size_t VGA_WIDTH = 120;
static const size_t VGA_HEIGHT = 45;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

static int X_FONTWIDTH = 8;
static int Y_FONTWIDTH = 16; 
static vec2 cursor_pos = { 0, 0 };

char kb_buffer[1024];

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	terminal_writelog("TTY Initialised");

	string init_string = "Terminal Initialised"; 
	init_string += string(" - C++ functions enabled! \n"); 

	terminal_writestring((char * )init_string); 
}

void terminal_writelog(char* data) {
	terminal_writestring("[");
	terminal_writestring("OK", RGBA(0x00FF00));
	terminal_writestring("] ");
	terminal_writestring(data);
	terminal_writestring("\n");
}

void terminal_writeerror(char* data) {
	terminal_writestring("[");
	terminal_writestring("ERR", RGBA(0xFF0000));
	terminal_writestring("] ");
	terminal_writestring(data);
	terminal_writestring("\n");
}

void terminal_scrollup(void) {
	terminal_row = VGA_HEIGHT-1;

	for(auto it = screen_surfaces.begin(); it != screen_surfaces.end(); it++) {
		(*it).scrollUp(Y_FONTWIDTH+1);
	}

	update_buffer(true);
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(const char c, RGBA color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;

	terminal_buffer[index] = vga_entry(c, VGA_COLOR_WHITE);

	drawchar_transparent(c, x*X_FONTWIDTH, y*Y_FONTWIDTH, color);
}

void terminal_clear(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}

	for(size_t x = 0; x < frame_width; x++) {
		for(size_t y = 0; y < frame_height; y++) {
			setpx(x, y, color_black);
		}
	}
}


void terminal_putchar(const char c, RGBA color) {
	if(c == '\n') {
		terminal_column = 0;
		terminal_row++;
		return;
	} 

	if(c == '\t') {
		// support tabs
		terminal_column += TAB_WIDTH - terminal_column % TAB_WIDTH;
		return;
	}

	terminal_putentryat(c, color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}

	update_cursor(terminal_row, terminal_column);
}

void terminal_writestring(const char* data) {
	if(terminal_row == VGA_HEIGHT) {
		terminal_scrollup();
	}

	for (; *data; ++data) {
		terminal_putchar((*data), RGBA(0xFFFFFF));
	}
}

void terminal_writestring(const char* data, RGBA color) {
	if(terminal_row == VGA_HEIGHT) {
		terminal_scrollup();
	}

	for (; *data; ++data) {
		terminal_putchar((*data), color);
	}
}

void terminal_multistring(char* data1, RGBA color1, char* data2, RGBA color2) {
	terminal_writestring(data1, color1);
	terminal_writestring(data2, color2);
}

void terminal_printf(const char* fmt, ...) {
	va_list parameters;
	char temp_buffer[80] = {0};

	va_start(parameters, fmt);
	vsprintf(temp_buffer, fmt, parameters);
	terminal_writestring((char*)temp_buffer);
	va_end(parameters);
}

void terminal_printf_rgba(const char* fmt, RGBA color, ...) {
	va_list parameters;
	char temp_buffer[80] = {0};

	va_start(parameters, color);
	vsprintf(temp_buffer, fmt, parameters);
	terminal_writestring((char*)temp_buffer, color);
	va_end(parameters);
}


size_t tty_get_cursor_x() {
	return terminal_column;
}

void tty_set_cursor_x(size_t x) {
	terminal_column = x;

	update_cursor(terminal_row, x);
}

void update_cursor(int row, int col)
{
	unsigned short position=(row*80) + col;

	terminal_putentryat('|', RGBA(0x2a2b31), cursor_pos.x, cursor_pos.y); // remove old caret

	terminal_putentryat('|', RGBA(0x00FF00), col+1, row);

	cursor_pos.x = col+1;
	cursor_pos.y = row;

	// cursor LOW port to vga INDEX register
	outportb(0x3D4, 0x0F);
	outportb(0x3D5, (unsigned char)(position&0xFF));
	
	// cursor HIGH port to vga INDEX register
	outportb(0x3D4, 0x0E);
	outportb(0x3D5, (unsigned char )((position>>8)&0xFF));

}

void tty_update() {
	terminal_writestring("[", RGBA(0xe4e4c8));
	terminal_writestring((char*)ENV::get("user"), RGBA(0xff6064));
	terminal_writestring("@", RGBA(0xff6064));
	terminal_writestring((char*)ENV::get("comp-name"), RGBA(0xff6064));
	terminal_writestring(" ", RGBA(0xff6064));

	terminal_writestring("0:", RGBA(0x288acc));
	terminal_writestring((char*)ENV::get("cd"), RGBA(0x288acc));
	
	terminal_writestring("] ", RGBA(0xe4e4c8));
	update_buffer(false);

	getsn(&kb_buffer[0], 1024);

	Command::Parse(kb_buffer);
}