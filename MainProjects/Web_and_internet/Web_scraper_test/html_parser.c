#include "html_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

html_node_t* html_node_create(html_node_type_t type) {
    html_node_t* node = calloc(1, sizeof(html_node_t));
    if (!node) return NULL;

    node->type = type;
    node->attribute_count = 0;
    node->child_count = 0;
    node->parent = NULL;

    return node;
}

void html_node_free(html_node_t* node) {
    if (!node) return;

    for (int i = 0; i < node->child_count; i++) {
        html_node_free(node->children[i]);
    }

    free(node);
}

void html_node_add_child(html_node_t* parent, html_node_t* child) {
    if (!parent || !child || parent->child_count >= MAX_CHILDREN) return;

    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

void html_node_add_attribute(html_node_t* node, const char* name, const char* value) {
    if (!node || !name || node->attribute_count >= MAX_ATTRIBUTES) return;

    html_attribute_t* attr = &node->attributes[node->attribute_count++];
    strncpy(attr->name, name, MAX_ATTR_NAME - 1);
    attr->name[MAX_ATTR_NAME - 1] = '\0';

    if (value) {
        strncpy(attr->value, value, MAX_ATTR_VALUE - 1);
        attr->value[MAX_ATTR_VALUE - 1] = '\0';
    } else {
        attr->value[0] = '\0';
    }
}

const char* html_node_get_attribute(html_node_t* node, const char* name) {
    if (!node || !name) return NULL;

    for (int i = 0; i < node->attribute_count; i++) {
        if (strcmp(node->attributes[i].name, value) == 0) {
            return node->attributes[i].value;
        }
    }

    return NULL;
}

html_parser_t* html_parser_create(const char* html) {
    if (!html) return NULL;

    html_parser_t* parser = malloc(sizeof(html_parser_t));
    if (!parser) return NULL;

    parser->html = html;
    parser->length = strlen(html);
    parser->position = 0;
    parser->line = 1;
    parser->column = 1;

    return parser;
}

void html_parser_free(html_parser_t* parser) {
    if (parser) {
        free(parser);
    }
}

void html_parser_skip_whitespace(html_parser_t* parser) {
    while (parser->position < parser->length && isspace(parser->html[parser->position])) {
        if (parser->html[parser->position] == '\n') {
            parser->line++;
            parser->column = 1;
        } else {
            parser->column++;
        }
        parser->position++;
    }
}

int html_parser_peek_char(html_parser_t* parser) {
    if (parser->position >= parser->length) return -1;
    return parser->html[parser->position];
}

int html_parser_next_char(html_parser_t* parser) {
    if (parser->position >= parser->length) return -1;

    int ch = parser->html[parser->position++];
    if (ch == '\n') {
        parser->line++;
        parser->column = 1;
    } else {
        parser->column++;
    }

    return ch;
}

int html_parser_match_string(html_parser_t* parser, const char* str) {
    size_t len = strlen(str);
    if (parser->position + len > parser->length) return 0;

    return strncmp(parser->html + parser->position, str, len) == 0;
}

char* html_parse_text(html_parser_t* parser) {
    if (!parser) return NULL;

    size_t start = parser->position;

    while (parser->position < parser->length && parser->html[parser->position] != '<') {
        parser->position++;
    }

    if (parser->position == start) return NULL;

    size_t len = parser->position - start;
    char* text = malloc(len + 1);
    if (!text) return NULL;

    strncpy(text, parser->html + start, len);
    text[len] = '\0';

    char* trimmed = text;
    while (isspace(*trimmed)) trimmed++;

    char* end = text + strlen(text) - 1;
    while (end > trimmed && isspace(*end)) *end-- = '\0';

    if (*trimmed == '\0') {
        free(text);
        return NULL;
    }

    char* result = malloc(strlen(trimmed) + 1);
    strcpy(result, trimmed);
    free(text);
}

html_node_t* html_parse_element(html_parser_t* parser) {
    if (!parser || html_parser_peek_char(parser) != '<') return NULL;

    html_parser_next_char(parser);

    if (html_parser_match_string(parser, "!--")) {
        parser->position += 3;
        while (parser->position < parser->length - 2) {
            if (html_parser_match_string(parser, "-->")) {
                parser->position += 3;
                break;
            }
            parser->position++;
        }
        return NULL;
    }

    html_node_t* node = html_node_create(HTML_NODE_ELEMENT);
    if (!node) return NULL;

    html_parser_skip_whitespace(parser);

    size_t tag_start = parser->position;
    while (parser->position < parser->length &&
        !isspace(parser->html[parser->position]) &&
        parser->html[parser->position] != '>' &&
        parser->html[parser->position] != '/') {
            parser->position++;
        }

    size_t tag_len = parser->position - tag_start;
    if (tag_len >= MAX_TAG_NAME) tag_len = MAX_TAG_NAME - 1;
    strncpy(node->tag_name, parser->html + tag_start, tag_len);
    node->tag_name[tag_len] = '\0';

    html_parser_skip_whitespace(parser);

    while (parser->position < parser->length &&
        parser->html[parser->position] != '>' &&
        parser->html[parser->position] != '/') {
            
            html_parser_skip_whitespace(parser);

            size_t attr_start = parser->position;
            while (parser->position < parser->length &&
                !isspace(parser->html[parser->position]) &&
                parser->html[parser->position] != '=' &&
                parser->html[parser->position] != '>' &&
                parser->html[parser->position] != '/') {
                    parser->position++;
                }

                if (parser->position == attr_start) break;

                size_t attr_len = parser->position - attr_start;
                char attr_name[MAX_ATTR_NAME];
                if (attr_len >= MAX_ATTR_NAME) attr_len = MAX_ATTR_NAME - 1;
                strncpy(attr_name, parser->html + attr_start, attr_len);
                attr_name[attr_len] = '\0';

                html_parser_skip_whitespace(parser);

                char attr_value[MAX_ATTR_VALUE] = {0};
                if (html_parser_peek_char(parser) == '=') {
                    html_parser_next_char(parser);
                    html_parser_skip_whitespace(parser);

                    int quote_char = html_parser_peek_char(parser);
                    if (quote_char == '"' || quote_char == '\'') {
                        html_parser_next_char(parser);

                        size_t value_start = parser->position;
                        while (parser->position < parser->length &&
                            parser->html[parser->position] != quote_char) {
                                parser->position++;
                            }

                            size_t value_len = parser->position - value_start;
                            if (value_len >= MAX_ATTR_VALUE) value_len = MAX_ATTR_VALUE - 1;
                            strncpy(attr_value, parser->html + value_start, value_len);
                            attr_value[value_len] = '\0';

                            if (parser->position < parser->length) {
                                html_parser_next_char(parser);
                            }
                    }
                }

                html_node_add_attribute(node, attr_name, attr_value);
        }

        int is_self_closing = 0;
        if (html_parser_peek_char(parser) == '/') {
            is_self_closing = 1;
            html_parser_next_char(parser);
        }
}
