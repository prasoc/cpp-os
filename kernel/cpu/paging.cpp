#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <tty.h>

#include <cpu/io.h>
#include <cpu/paging.h>

#include <gfx/vesa.h>

#define PAGE_SIZE 4096

// aligned to 4KiB boundary is a HACK - we need to create a page frame allocator!
// at the moment, this means we can _only_ use GCC to compile the kernel... yuck :(
PageDirectory_t page_directory[1024] __attribute__((aligned(4096))); // the page directory takes up 4KiB of memory.
PageTable_t page_table[1024][1024] __attribute__((aligned(4096)));

extern "C" void set_paging_bit(); // asm global function
extern "C" void set_paging_pointer(PageDirectory_t*); // asm global function

void* get_phys_addr(void* virt_addr) {
    
}

// one page table array gives us 4MiB of usuable virtual address space
void setup_single_page(int pg_dir_index, PageTable_t pg_table[1024]) {
    for(int i = 0; i < 1024; i++) {
        pg_table[i].present = 1;
        pg_table[i].rw = 1;
        pg_table[i].access_lvl = 0;
        pg_table[i].physical_address = pg_dir_index*0x400 + i;
    } 
}

// "map_page"

// "free_page"
// "alloc_page"

// "first_page"

// void map_page(PageTable_t pg_tbl[1024], void *addr) {
//     for(size_t i=0; i<4096; i++)
//     {
//         pg_tbl[i].rw   =1;
//         pg_tbl[i].access_lvl=0;
//         pg_tbl[i].address =((int)addr)/4096 + i;
//         pg_tbl[i].present =1;
//     }
// }

void init_paging_dir(PageDirectory_t pg_dir[1024]) {
    for(int i = 0; i < 1024; i++) {
        pg_dir[i].present = 0;
        pg_dir[i].rw = 1;
        pg_dir[i].access_lvl = 0;
    }
}

void init_paging_table(PageDirectory_t pg_dir[1024], PageTable_t pg_table[1024][1024]) {
    for(int i = 0; i < 1024; i++) {
        setup_single_page(i, pg_table[i]);
    }
}

void paging_install() {

    init_paging_dir(page_directory);
    init_paging_table(page_directory, page_table);

    for(int i=0; i<1024; i++)
    {
        page_directory[i].present    = 1;
        page_directory[i].rw    = 1;
        page_directory[i].access_lvl = 0;
        page_directory[i].table_address   = (uint32_t)(page_table[i]) >> 12;
    }

    // // set up cr3 to point to the beginning address of our paging directories
    set_paging_pointer(page_directory);

    // cr0 bit 31 gets set
    set_paging_bit();
    
}

