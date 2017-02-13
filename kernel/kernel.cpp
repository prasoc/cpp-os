#include <stddef.h> 
#include <stdint.h> 

#include <string.h> 
#include <tty.h>

#include <cpu/multiboot.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>

#include <gfx/vesa.h>

#include <devices/keyboard.h>

extern "C"
void kernel_main(multiboot_info_t * mb_info, uint32_t stack_size, uintptr_t esp) {
	interrupts_disable();

	init_fbe(mb_info);

	terminal_initialize(); 
	terminal_writestring("Hello, kernel World!\n"); 

	gdt_install();
	idt_install();

	keyboard_install();

	interrupts_enable();

	char buffer[1024];

	while (true) {
		terminal_writestring_color("kernel", VGA_COLOR_LIGHT_GREY);
		terminal_writestring_color("> ", VGA_COLOR_WHITE);
		update_buffer();

		getsn(&buffer[0], 1024);

		// if (strncmp(buffer, "clear", 5) == 0) {
		// 	terminal_clear();
		// } 
		// else if (strncmp(buffer, "help", 4) == 0) {
		// 	string help = "List of commands\n"
		// 		"help:        prints a list of commands\n"
		// 		"clear:       clears the screen\n";

		// 	terminal_writestring((char*)help);
		// }
		// if (strncmp(buffer, "ls", 5) == 0) {
		// } 

	}
}