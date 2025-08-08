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
        if (strcmp(node->attributes[i].name, name) == 0) {
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

    // Trim whitespace
    char* trimmed = text;
    while (isspace(*trimmed)) trimmed++;

    char* end = text + strlen(text) - 1;
    while (end > trimmed && isspace(*end)) *end-- = '\0';

    if (*trimmed == '\0') {
        free(text);
        return NULL;
    }

    char* result = malloc(strlen(trimmed) + 1);
    if (result) {
        strcpy(result, trimmed);
    }
    free(text);
    return result;
}

html_node_t* html_parse_element(html_parser_t* parser) {
    if (!parser || html_parser_peek_char(parser) != '<') return NULL;

    html_parser_next_char(parser); // consume '<'

    // Handle comments
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

    // Parse tag name
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

    // Parse attributes
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
            html_parser_next_char(parser); // consume '='
            html_parser_skip_whitespace(parser);

            int quote_char = html_parser_peek_char(parser);
            if (quote_char == '"' || quote_char == '\'') {
                html_parser_next_char(parser); // consume quote

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
                    html_parser_next_char(parser); // consume closing quote
                }
            }
        }

        html_node_add_attribute(node, attr_name, attr_value);
    }

    // Check for self-closing tag
    int is_self_closing = 0;
    if (html_parser_peek_char(parser) == '/') {
        is_self_closing = 1;
        html_parser_next_char(parser);
    }

    if (html_parser_peek_char(parser) == '>') {
        html_parser_next_char(parser);
    }

    // Check if it's a void element or self-closing
    if (is_self_closing ||
        strcmp(node->tag_name, "br") == 0 ||
        strcmp(node->tag_name, "hr") == 0 ||
        strcmp(node->tag_name, "img") == 0 ||
        strcmp(node->tag_name, "input") == 0 ||
        strcmp(node->tag_name, "meta") == 0 ||
        strcmp(node->tag_name, "link") == 0 ||
        strcmp(node->tag_name, "area") == 0 ||
        strcmp(node->tag_name, "base") == 0 ||
        strcmp(node->tag_name, "col") == 0 ||
        strcmp(node->tag_name, "embed") == 0 ||
        strcmp(node->tag_name, "source") == 0 ||
        strcmp(node->tag_name, "track") == 0 ||
        strcmp(node->tag_name, "wbr") == 0) {
        return node;
    }

    // Parse children
    while (parser->position < parser->length) {
        html_parser_skip_whitespace(parser);

        if (html_parser_peek_char(parser) == '<') {
            if (html_parser_match_string(parser, "</")) {
                parser->position += 2;
                html_parser_skip_whitespace(parser);

                if (strncmp(parser->html + parser->position, node->tag_name, strlen(node->tag_name)) == 0) {
                    parser->position += strlen(node->tag_name);
                    html_parser_skip_whitespace(parser);
                    if (html_parser_peek_char(parser) == '>') {
                        html_parser_next_char(parser);
                    }
                    break;
                } else {
                    parser->position -= 2; // backtrack
                }
            }

            html_node_t* child = html_parse_element(parser);
            if (child) {
                html_node_add_child(node, child);
            }
        } else {
            char* text = html_parse_text(parser);
            if (text) {
                html_node_t* text_node = html_node_create(HTML_NODE_TEXT);
                if (text_node) {
                    strncpy(text_node->text_content, text, MAX_TEXT_CONTENT - 1);
                    text_node->text_content[MAX_TEXT_CONTENT - 1] = '\0';
                    html_node_add_child(node, text_node);
                }
                free(text);
            } else {
                break;
            }
        }
    }

    return node;
}

html_node_t* html_parse_document(html_parser_t* parser) {
    if (!parser) return NULL;

    html_node_t* root = html_node_create(HTML_NODE_ELEMENT);
    if (!root) return NULL;

    strcpy(root->tag_name, "document");

    while (parser->position < parser->length) {
        html_parser_skip_whitespace(parser);

        if (parser->position >= parser->length) break;

        if (html_parser_peek_char(parser) == '<') {
            html_node_t* child = html_parse_element(parser);
            if (child) {
                html_node_add_child(root, child);
            }
        } else {
            char* text = html_parse_text(parser);
            if (text) {
                html_node_t* text_node = html_node_create(HTML_NODE_TEXT);
                if (text_node) {
                    strncpy(text_node->text_content, text, MAX_TEXT_CONTENT - 1);
                    text_node->text_content[MAX_TEXT_CONTENT - 1] = '\0';
                    html_node_add_child(root, text_node);
                }
                free(text);
            } else {
                break;
            }
        }
    }

    return root;
}

html_node_list_t* html_find_elements_by_tag(html_node_t* root, const char* tag_name) {
    if (!root || !tag_name) return NULL;

    html_node_list_t* list = malloc(sizeof(html_node_list_t));
    if (!list) return NULL;

    list->capacity = 16;
    list->nodes = malloc(list->capacity * sizeof(html_node_t*));
    list->count = 0;

    if (!list->nodes) {
        free(list);
        return NULL;
    }

    void find_recursive(html_node_t* node) {
        if (node->type == HTML_NODE_ELEMENT && strcmp(node->tag_name, tag_name) == 0) {
            if (list->count >= list->capacity) {
                list->capacity *= 2;
                html_node_t** new_nodes = realloc(list->nodes, list->capacity * sizeof(html_node_t*));
                if (new_nodes) {
                    list->nodes = new_nodes;
                }
            }
            if (list->count < list->capacity) {
                list->nodes[list->count++] = node;
            }
        }

        for (int i = 0; i < node->child_count; i++) {
            find_recursive(node->children[i]);
        }
    }

    find_recursive(root);
    return list;
}

html_node_list_t* html_find_elements_by_class(html_node_t* root, const char* class_name) {
    if (!root || !class_name) return NULL;

    html_node_list_t* list = malloc(sizeof(html_node_list_t));
    if (!list) return NULL;

    list->capacity = 16;
    list->nodes = malloc(list->capacity * sizeof(html_node_t*));
    list->count = 0;

    if (!list->nodes) {
        free(list);
        return NULL;
    }

    void find_recursive(html_node_t* node) {
        if (node->type == HTML_NODE_ELEMENT) {
            const char* class_attr = html_node_get_attribute(node, "class");
            if (class_attr) {
                // Check if class_name is in the class attribute (space-separated values)
                char* class_copy = malloc(strlen(class_attr) + 1);
                if (class_copy) {
                    strcpy(class_copy, class_attr);
                    char* token = strtok(class_copy, " ");
                    while (token) {
                        if (strcmp(token, class_name) == 0) {
                            if (list->count >= list->capacity) {
                                list->capacity *= 2;
                                html_node_t** new_nodes = realloc(list->nodes, list->capacity * sizeof(html_node_t*));
                                if (new_nodes) {
                                    list->nodes = new_nodes;
                                }
                            }
                            if (list->count < list->capacity) {
                                list->nodes[list->count++] = node;
                            }
                            break;
                        }
                        token = strtok(NULL, " ");
                    }
                    free(class_copy);
                }
            }
        }

        for (int i = 0; i < node->child_count; i++) {
            find_recursive(node->children[i]);
        }
    }

    find_recursive(root);
    return list;
}

html_node_t* html_find_element_by_id(html_node_t* root, const char* id) {
    if (!root || !id) return NULL;

    if (root->type == HTML_NODE_ELEMENT) {
        const char* id_attr = html_node_get_attribute(root, "id");
        if (id_attr && strcmp(id_attr, id) == 0) {
            return root;
        }
    }

    for (int i = 0; i < root->child_count; i++) {
        html_node_t* result = html_find_element_by_id(root->children[i], id);
        if (result) return result;
    }

    return NULL;
}

void html_node_list_free(html_node_list_t* list) {
    if (list) {
        if (list->nodes) {
            free(list->nodes);
        }
        free(list);
    }
}

void html_print_node(html_node_t* node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  ");

    if (node->type == HTML_NODE_ELEMENT) {
        printf("<%s", node->tag_name);
        for (int i = 0; i < node->attribute_count; i++) {
            printf(" %s=\"%s\"", node->attributes[i].name, node->attributes[i].value);
        }
        printf(">\n");

        for (int i = 0; i < node->child_count; i++) {
            html_print_node(node->children[i], indent + 1);
        }
    } else if (node->type == HTML_NODE_TEXT) {
        printf("TEXT: %s\n", node->text_content);
    }
}

char* html_node_get_text_content(html_node_t* node) {
    if (!node) return NULL;

    if (node->type == HTML_NODE_TEXT) {
        char* result = malloc(strlen(node->text_content) + 1);
        if (result) {
            strcpy(result, node->text_content);
        }
        return result;
    }

    // Calculate total length needed
    size_t total_length = 0;
    for (int i = 0; i < node->child_count; i++) {
        char* child_text = html_node_get_text_content(node->children[i]);
        if (child_text) {
            total_length += strlen(child_text);
            free(child_text);
        }
    }

    if (total_length == 0) return NULL;

    char* result = malloc(total_length + 1);
    if (!result) return NULL;

    result[0] = '\0';
    for (int i = 0; i < node->child_count; i++) {
        char* child_text = html_node_get_text_content(node->children[i]);
        if (child_text) {
            strcat(result, child_text);
            free(child_text);
        }
    }

    return result;
}
