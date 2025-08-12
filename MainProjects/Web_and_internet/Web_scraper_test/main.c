#include "web_scraper.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    console_t* console = console_init();
    if (!console) {
        printf("Failed to initialize console\n");
        return 1;
    }
    
    if (http_init() != 0) {
        console_print_status(console, "Failed to initialize network", 0);
        console_cleanup(console);
        return 1;
    }
    
    console_clear(console);
    console_print_header(console, "Web Scraper Application");
    console_print(console, "Welcome to the Web Scraper!\n");
    console_print(console, "This tool allows you to extract data from web pages.\n\n");
    console_wait_key(console, "Press any key to continue...");
    
    web_scraper_t* scraper = scraper_create(console);
    if (!scraper) {
        console_print_status(console, "Failed to create scraper", 0);
        http_cleanup();
        console_cleanup(console);
        return 1;
    }
    
    scraper_show_main_menu(scraper);
    
    console_clear(console);
    console_center_text(console, 10, "Thank you for using Web Scraper!");
    console_center_text(console, 11, "Goodbye!");
    console_goto_xy(console, 0, 13);
    
    scraper_free(scraper);
    http_cleanup();
    console_cleanup(console);
    
    return 0;
}