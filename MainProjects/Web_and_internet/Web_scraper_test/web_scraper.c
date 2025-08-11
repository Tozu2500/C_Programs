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

