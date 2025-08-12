#ifndef WEB_SCRAPER_H
#define WEB_SCRAPER_H

#include "http_client.h"
#include "html_parser.h"
#include "console_ui.h"

#define MAX_SCRAPED_ITEMS 1000
#define MAX_SELECTORS 16
#define MAX_SELECTOR_LENGTH 256

typedef struct {
    char text[512];
    char url[512];
    char image_url[512];
    char additional_data[1024];
} scraped_item_t;

typedef struct {
    char name[128];
    char css_selector[MAX_SELECTOR_LENGTH];
    char attribute[64];
    int extract_text;
    int extract_href;
    int extract_src;
} scraper_rule_t;

typedef struct {
    char url[MAX_URL_LENGTH];
    char name[256];
    scraper_rule_t rules[MAX_SELECTORS];
    int rule_count;
    scraped_item_t items[MAX_SCRAPED_ITEMS];
    int item_count;
    int max_items;
    int timeout_ms;
} scraper_config_t;

typedef struct {
    console_t* console;
    scraper_config_t config;
    http_connection_t connection;
    html_node_t* document;
    int is_running;
} web_scraper_t;

web_scraper_t* scraper_create(console_t* console);
void scraper_free(web_scraper_t* scraper);

int scraper_configurer(web_scraper_t* scraper);
int scraper_add_rule(web_scraper_t* scraper, const char* name, const char* selector, const char* attribute);
int scraper_run(web_scraper_t* scraper);

int scraper_fetch_page(web_scraper_t* scraper, const char* url);
int scraper_extract_data(web_scraper_t* scraper);
void scraper_save_results(web_scraper_t* scraper);
void scraper_display_results(web_scraper_t* scraper);

html_node_list_t* scraper_select_elements(html_node_t* root, const char* selector);
char* scraper_extract_text(html_node_t* node);
char* scraper_extract_attribute(html_node_t* node, const char* attribute);

void scraper_show_config_menu(web_scraper_t* scraper);
void scraper_show_results_menu(web_scraper_t* scraper);
void scraper_show_main_menu(web_scraper_t* scraper);

int scraper_validate_url(const char* url);
void scraper_print_statistics(web_scraper_t* scraper);

#endif // WEB_SCRAPER_H