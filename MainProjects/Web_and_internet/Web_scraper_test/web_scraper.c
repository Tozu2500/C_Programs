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

    
}
