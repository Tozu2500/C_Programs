#include "web_scraper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

web_scraper_t* scraper_create(console_t* console) {
    if (!console) return NULL;

    web_scraper_t* scraper = malloc(sizeof(web_scraper_t));
    if (!scraper) return NULL;

    memset(scraper, 0, sizeof(web_scraper_t));
    scraper->console = console;
    scraper->config.max_items = 100;
    scraper->config.timeout_ms = 30000;
    scraper->is_running = 0;

    return scraper;
}

void scraper_free(web_scraper_t* scraper) {
    if (!scraper) return;

    if (scraper->document) {
        html_node_free(scraper->document);
    }

    if (scraper->connection.is_connected) {
        http_disconnect(&scraper->connection);
    }

    free(scraper);
}

int scraper_validate_url(const char* url) {
    if (!url || strlen(url) == 0) return 0;

    if (strncmp(url, "http://", 7) != 0 && strncmp(url, "https://", 8) != 0) {
        return 0;
    }

    return 1;
}

int scraper_configurer(web_scraper_t* scraper) {
    if (!scraper) return -1;

    console_clear(scraper->console);
    console_print_header(scraper->console, "Scraper configuration");

    char* input = console_get_input(scraper->console, "Enter target URL: ");
    if (!input || !scraper_validate_url(input)) {
        console_print_status(scraper->console, "Invalid URL format", 0);
        if (input) free(input);
        console_wait_key(scraper->console, NULL);
        return -1;
    }

    strncpy(scraper->config.url, input, sizeof(scraper->config.url) - 1);
    free(input);

    input = console_get_input(scraper->console, "Enter scraper name: ");
    if (input) {
        strncpy(scraper->config.name, input, sizeof(scraper->config.name) - 1);
        free(input);
    } else {
        strcpy(scraper->config.name, "Default scraper");
    }

    scraper->config.max_items = console_get_number(scraper->console,
    "Maximum items to scrape (1-1000): ", 1, MAX_SCRAPED_ITEMS);

    scraper->config.timeout_ms = console_get_number(scraper->console,
    "Timeout in seconds (5-120): ", 5, 120) * 1000;

    console_print_status(scraper->console, "Configuration saved", 1);
    console_wait_key(scraper->console, NULL);

    return 0;
}

int scraper_add_rule(web_scraper_t* scraper, const char* name, const char* selector, const char* attribute) {
    if (!scraper || !name || !selector || scraper->config.rule_count >= MAX_SELECTORS) {
        return -1;
    }

    scraper_rule_t* rule = &scraper->config.rules[scraper->config.rule_count++];

    strncpy(rule->name, name, sizeof(rule->name) - 1);
    rule->name[sizeof(rule->name) - 1] = '\0';

    strncpy(rule->css_selector, selector, sizeof(rule->css_selector) - 1);
    rule->css_selector[sizeof(rule->css_selector) - 1] = '\0';

    if (attribute) {
        strncpy(rule->attribute, attribute, sizeof(rule->attribute) - 1);
        rule->attribute[sizeof(rule->attribute) - 1] = '\0';

        rule->extract_href = (strcmp(attribute, "href") == 0);
        rule->extract_src = (strcmp(attribute, "src") == 0);
        rule->extract_text = 0;
    } else {
        rule->extract_text = 1;
        rule->extract_href = 0;
        rule->extract_src = 0;
        rule->attribute[0] = '\0';
    }

    return 0;
}

int scraper_fetch_page(web_scraper_t* scraper, const char* url) {
    if (!scraper || !url) return -1;

    url_components_t components;
    if (parse_url(url, &components) < 0) {
        return -1;
    }

    console_print(scraper->console, "Connecting to %s:%d...\n", components.host, components.port);

    if (http_connect(&scraper->connection, components.host, components.port, scraper->config.timeout_ms) < 0) {
        console_print_status(scraper->console, "Failed to connect", 0);
        return -1;
    }

    console_print_status(scraper->console, "Connected successfully", 1);
    console_print(scraper->console, "Sending HTTP request...\n");

    if (http_send_request(&scraper->connection, "GET", components.path, components.host, NULL) < 0) {
        console_print_status(scraper->console, "Failed to send request", 0);
        http_disconnect(&scraper->connection);
        return -1;
    }

    console_print(scraper->console, "Receiving response...\n");

    http_response_t response;
    if (http_receive_response(&scraper->connection, &response) < 0) {
        console_print_status(scraper->console, "Failed to receive response", 0);
        http_disconnect(&scraper->connection);
        return -1;
    }

    http_disconnect(&scraper->connection);

    int status_code = http_get_status_code(response.data);
    if (status_code != 200) {
        console_print_status(scraper->console, "HTTP error", 0);
        console_print(scraper->console, "Status code: %d\n", status_code);
        http_response_free(&response);
        return -1;
    }

    console_print_status(scraper->console, "Response received", 1);
    console_print(scraper->console, "Response size: %zu bytes\n", response.size);

    char* body = http_get_body(response.data);
    if (!body) {
        console_print_status(scraper->console, "No response body", 0);
        http_response_free(&response);
        return -1;
    }

    console_print(scraper->console, "Parsing HTML...\n");

    html_parser_t* parser = html_parser_create(body);
    if (!parser) {
        console_print_status(scraper->console, "Failed to create parser", 0);
        free(body);
        http_response_free(&response);
        return -1;
    }

    if (scraper->document) {
        html_node_free(scraper->document);
    }

    scraper->document = html_parse_document(parser);
    html_parser_free(parser);
    free(body);
    http_response_free(&response);

    if (!scraper->document) {
        console_print_status(scraper->console, "Failed to parse HTML", 0);
        return -1;
    }

    console_print_status(scraper->console, "HTML parser successfully", 1);
    return 0;
}

html_node_list_t* scraper_select_elements(html_node_t* root, const char* selector) {
    if (!root || !selector) return NULL;

    if (selector[0] == '#') {
        html_node_t* node = html_find_element_by_id(root, selector + 1);
        if (!node) return NULL;

        html_node_list_t* list = malloc(sizeof(html_node_list_t));
        if (!list) return NULL;

        list->nodes = malloc(sizeof(html_node_t*));
        if (!list->nodes) {
            free(list);
            return NULL;
        }

        list->nodes[0] = node;
        list->count = 1;
        list->capacity = 1;
        return list;
    }

    if (selector[0] == '.') {
        return html_find_elements_by_class(root, selector + 1);
    }

    return html_find_elements_by_tag(root, selector);
}

char* scraper_extract_text(html_node_t* node) {
    if (!node) return NULL;

    return html_node_get_text_content(node);
}

char* scraper_extract_attribute(html_node_t* node, const char* attribute) {
    if (!node || !attribute) return NULL;

    const char* attr_value = html_node_get_attribute(node, attribute);
    if (!attr_value) return NULL;

    char* result = malloc(strlen(attr_value) + 1);
    if (result) {
        strcpy(result, attr_value);
    }

    return result;
}

int scraper_extract_data(web_scraper_t* scraper) {
    if (!scraper || !scraper->document) return -1;

    console_print(scraper->console, "Extracting data using %d rules...\n", scraper->config.rule_count);

    scraper->config.item_count = 0;

    for (int rule_idx = 0; rule_idx < scraper->config.rule_count; rule_idx++) {
        scraper_rule_t* rule = &scraper->config.rules[rule_idx];

        console_print(scraper->console, "Applying rule: %s (%s)\n", rule->name, rule->css_selector);

        html_node_list_t* elements = scraper_select_elements(scraper->document, rule->css_selector);
        if (!elements) {
            console_print(scraper->console, "No elements found for rule: %s\n", rule->name);
            continue;
        }

        console_print(scraper->console, "Found %d elements\n", elements->count);

        for (int i = 0; i < elements->count && scraper->config.item_count < scraper->config.max_items; i++) {
            html_node_t* element = elements->nodes[i];
            scraper_item_t* item = &scraper->config.items[scraper->config.item_count];

            if (rule->extract_text) {
                char* text = scraper_extract_text(element);
                if (text) {
                    strncpy(item->text, text, sizeof(item->text) - 1);
                    item->text[sizeof(item->text) - 1] = '\0';
                    free(text);
                }
            }

            if (rule->extract_href) {
                char* href = scraper_extract_attribute(element, "href");
                if (href) {
                    strncpy(item->url, href, sizeof(item->url) - 1);
                    item->url[sizeof(item->url) - 1] = '\0';
                    free(href);
                }
            }

            if (rule->extract_src) {
                char* src = scraper_extract_attribute(element, "src");
                if (src) {
                    strncpy(item->image_url, src, sizeof(item->image_url) - 1);
                    item->image_url[sizeof(item->image_url) - 1] = '\0';
                    free(src);
                }
            }

            if (rule->attribute[0] != '\0' && !rule->extract_href && !rule->extract_src) {
                char* attr_value = scraper_extract_attribute(element, rule->attribute);
                if (attr_value) {
                    strncpy(item->additional_data, attr_value, sizeof(item->additional_data) - 1);
                    item->additional_data[sizeof(item->additional_data) - 1] = '\0';
                    free(attr_value);
                }
            }

            scraper->config.item_count++;
            console_show_progress(scraper->console, scraper->config.item_count, scraper->config.max_items, "Extracting");
        }

        html_node_list_free(elements);
    }

    console_print_status(scraper->console, "Data extraction completed", 1);
    console_print(scraper->console, "Extracted %d items\n", scraper->config.item_count);

    return scraper->config.item_count;
}

void scraper_save_results(web_scraper_t* scraper) {
    if (!scraper || scraper->config.item_count == 0) return;

    char filename[256];
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);

    strftime(filename, sizeof(filename), "scraped_data_%Y%m%d_%H%M%S.txt", tm_info);

    FILE* file = fopen(filename, "w");
    if (!file) {
        console_print_status(scraper->console, "Failed to create output file", 0);
        return;
    }

    fprintf(file, "Web scraping results\n");
    fprintf(file, "==================\n");
    fprintf(file, "URL: %s\n", scraper->config.url);
    fprintf(file, "Scraper: %s\n", scraper->config.name);
    fprintf(file, "Items: %d\n", scraper->config.item_count);
    fprintf(file, "Generated: %s\n\n", ctime(&now));

    for (int i = 0; i < scraper->config.item_count; i++) {
        scraped_item_t* item = &scraper->config.items[i];

        fprintf(file, "Item %d:\n", i + 1);
        if (item->text[0]) fprintf(file, "  Text: %s\n", item->text);
        if (item->url[0]) fprintf(file, "  URL: %s\n", item->url);
        if (item->image_url[0]) fprintf(file, "  Image: %s\n", item->image_url);
        if (item->additional_data[0]) fprintf(file, "  Data: %s\n", item->additional_data);
        fprintf(file, "\n");
    }

    fclose(file);

    console_print_status(scraper->console, "Results saved", 1);
    console_print(scraper->console, "Filename: %s\n", filename);
}

void scraper_display_results(web_scraper_t* scraper) {
    if (!scraper || scraper->config.item_count == 0) {
        console_print_status(scraper->console, "No results to display", 0);
        return;
    }

    console_clear(scraper->console);
    console_print_header(scraper->console, "Scraping results");

    int start = 0;
    int items_per_page = 10;

    while (1) {
        console_goto_xy(scraper->console, 0, 4);

        int end = start + items_per_page;
        if (end > scraper->config.item_count) end = scraper->config.item_count;

        console_print(scraper->console, "Showing items %d-%d of %d\n\n",
        start + 1, end, scraper->config.item_count);

        for (int i = start; i < end; i++) {
            scraped_item_t* item = &scraper->config.items[i];

            console_print_colored(scraper->console, COLOR_YELLOW, COLOR_BLACK, "Item %d:\n", i + 1);

            if (item->text[0]) {
                console_print(scraper->console, "  Text: %.80s%s\n", item->text, strlen(item->text) > 80 ? "..." : "");
            }

            if (item->url[0]) {
                console_print(scraper->console, "  URL: %.80s%s\n", item->url, strlen(item->url) > 80 ? "..." : "");
            }

            if (item->image_url[0]) {
                console_print(scraper->console, "  Image: %.80s%s\n", item->image_url, strlen(item->image_url) > 80 ? "..." : "");
            }

            if (item->additional_data[0]) {
                console_print(scraper->console, "  Data: %.80s%s\n", item->additional_data, strlen(item->additional_data) > 80 ? "..." : "");
            }

            console_print(scraper->console, "\n");
        }

        console_print(scraper->console, "[N]ext page | [P]revious page | [S]ave | [Q]uit\n");

        int key = _getch();
        switch (key) {
            case 'n':
            case 'N':
                if (end < scraper->config.item_count) {
                    start += items_per_page;
                }
                break;
            case 'p':
            case 'P':
                if (start > 0) {
                    start -= items_per_page;
                    if (start < 0) start = 0;
                }
                break;
            case 's':
            case 'S':
                scraper_save_results(scraper);
                console_wait_key(scraper->console, NULL);
                break;
            case 'q':
            case 'Q':
                return;
        }

        console_clear(scraper->console);
        console_print_header(scraper->console, "Scraping results");
    }
}

void scraper_show_config_menu(web_scraper_t* scraper) {
    if (!scraper) return;

    menu_t* menu = menu_create("Configuration Menu");
    menu_add_item(menu, "Set Target URL", 1);
    menu_add_item(menu, "Add Extraction Rule", 2);
    menu_add_item(menu, "View Current Rules", 3);
    menu_add_item(menu, "Clear All Rules", 4);
    menu_add_item(menu, "Set Limits", 5);
    menu_add_item(menu, "Back to Main Menu", 0);

    while (1) {
        int choice = menu_show(scraper->console, menu);

        switch (choice) {
            case 1: {
                char* url = console_get_input(scraper->console, "Enter URL: ");
                if (url && scraper_validate_url(url)) {
                    strncpy(scraper->config.url, url, sizeof(scraper->config.url) - 1);
                    console_print_status(scraper->console, "URL updated", 1);
                } else {
                    console_print_status(scraper->console, "Invalid URL", 0);
                }

                if (url) free(url);
                console_wait_key(scraper->console, NULL);
                break;
            }

            case 2: {
                if (scraper->config.rule_count >= MAX_SELECTORS) {
                    console_print_status(scraper->console, "Maximum rules reached", 0);
                    console_wait_key(scraper->console, NULL);
                    break;
                }

                char* name = console_get_input(scraper->console, "Rule name: ");
                char* selector = console_get_input(scraper->console, "CSS selector: ");
                char* attribute = console_get_input(scraper->console, "Attribute (or leave empty for text): ");

                if (name && selector) {
                    if (scraper_add_rule(scraper, name, selector,
                                        (attribute && strlen(attribute) > 0) ? attribute : NULL) == 0) {
                                            console_print_status(scraper->console, "Rule added", 1);
                                        }
                } else {
                    console_print_status(scraper->console, "Failed to add rule", 0);
                }

            if (name) free(name);
            if (selector) free(selector);
            if (attribute) free(attribute);
            console_wait_key(scraper->console, NULL);
            break;
        }

        case 3: {
            console_clear(scraper->console);
            console_print_header(scraper->console, "Current rules");

            if (scraper->config.rule_count == 0) {
                console_print(scraper->console, "No rules configured\n");
            } else {
                for (int i = 0; i < scraper->config.rule_count; i++) {
                    scraper_rule_t* rule = &scraper->config.rules[i];
                    console_print(scraper->console, "%d. %s\n", i + 1, rule->name);
                    console_print(scraper->console, "   Selector: %s\n", rule->css_selector);

                    if (rule->attribute[0]) {
                        console_print(scraper->console, "   Attribute: %s\n", rule->attribute);
                    } else {
                        console_print(scraper->console, "   Extract: text content\n");
                    }
                    console_print(scraper->console, "\n");
                }
            }
            console_wait_key(scraper->console, NULL);
            break;
        }

        case 4: {
            if (console_get_yes_no(scraper->console, "Clear all rules? (y/n): ")) {
                scraper->config.rule_count = 0;
                console_print_status(scraper->console, "All rules cleared", 1);
            }
            console_wait_key(scraper->console, NULL);
            break;
        }

        case 5: {
            scraper->config.max_items = console_get_number(scraper->console,
            "Maximum items (1-1000): ", 1, MAX_SCRAPED_ITEMS);
            scraper->config.timeout_ms = console_get_number(scraper->console,
            "Timeout in seconds (5-120): ", 5, 120) * 1000;
            console_print_status(scraper->console, "Limits updated", 1);
            console_wait_key(scraper->console, NULL);
            break;
        }

        case 0:
        case -1:
            menu_free(menu);
            return;
        }
    }
}

void scraper_print_statistics(web_scraper_t* scraper) {
    if (!scraper) return;

    console_print(scraper->console, "\nScraper statistics:\n");
    console_print_separator(scraper->console);
    console_print(scraper->console, "Target URL: %s\n",
    scraper->config.url[0] ? scraper->config.url : "Not set");
    console_print(scraper->console, "Rules configured: %d\n", scraper->config.rule_count);
    console_print(scraper->console, "Items scraped: %d\n", scraper->config.item_count);
    console_print(scraper->console, "Max items: %d\n", scraper->config.max_items);
    console_print(scraper->console, "Timeout: %d seconds\n", scraper->config.timeout_ms / 1000);
    console_print_separator(scraper->console);
}

int scraper_run(web_scraper_t* scraper) {
    if (!scraper) return -1;

    if (!scraper->config.url[0]) {
        console_print_status(scraper->console, "No URL configured", 0);
        return -1;
    }

    if (scraper->config.rule_count == 0) {
        console_print_status(scraper->console, "No extraction rules configured", 0);
        return -1;
    }

    console_clear(scraper->console);
    console_print_header(scraper->console, "Running Web Scraper");

    scraper_print_statistics(scraper);

    console_print(scraper->console, "\nStarting scraper...\n");

    if (scraper_fetch_page(scraper, scraper->config.url) < 0) {
        return -1;
    }

    if (scraper_extract_data(scraper) < 0) {
        return -1;
    }

    console_print_status(scraper->console, "Scraping completed successfully", 1);
    console_wait_key(scraper->console, NULL);

    return 0;
}

void scraper_show_main_menu(web_scraper_t* scraper) {
    if (!scraper) return;

    menu_t* menu = menu_create("Web Scraper v1.0");
    menu_add_item(menu, "Quick Setup", 1);
    menu_add_item(menu, "Advanced Configuration", 2);
    menu_add_item(menu, "Run Scraper", 3);
    menu_add_item(menu, "View Results", 4);
    menu_add_item(menu, "Save Results", 5);
    menu_add_item(menu, "Statistics", 6);
    menu_add_item(menu, "Exit", 0);

    while (1) {
        int choice = menu_show(scraper->console, menu);

        switch (choice) {
            case 1:
                if (scraper_configure(scraper) == 0) {
                    scraper_add_rule(scraper, "Title", "title", NULL);
                    scraper_add_rule(scraper, "Links", "a," "href");
                    scraper_add_rule(scraper, "Images", "img", "src");
                }
                break;

            case 2:
                scraper_show_config_menu(scraper);
                break;

            case 3:
                scraper_run(scraper);
                break;

            case 4:
                scraper_display_results(scraper);
                break;
            
            case 5:
                scraper_save_results(scraper);
                console_wait_key(scraper->console, NULL);
                break;

            case 6:
                console_clear(scraper->console);
                console_print_header(scraper->config, "Statistics");
                scraper_print_statistics(scraper);
                console_wait_key(scraper->console, NULL);
                break;

            case 0:
            case -1:
                menu_free(menu);
                return;
        }
    }
}
