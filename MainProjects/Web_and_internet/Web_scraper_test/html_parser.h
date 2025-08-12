#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <stddef.h>

#define MAX_TAG_NAME 64
#define MAX_ATTR_NAME 128
#define MAX_ATTR_VALUE 512
#define MAX_TEXT_CONTENT 8192
#define MAX_ATTRIBUTES 32
#define MAX_CHILDREN 256

typedef enum {
    HTML_NODE_ELEMENT,
    HTML_NODE_TEXT,
    HTML_NODE_COMMENT
} html_node_type_t;

typedef struct html_attribute {
    char name[MAX_ATTR_NAME];
    char value[MAX_ATTR_VALUE];
} html_attribute_t;

typedef struct html_node {
    html_node_type_t type;
    char tag_name[MAX_TAG_NAME];
    char text_content[MAX_TEXT_CONTENT];
    html_attribute_t attributes[MAX_ATTRIBUTES];
    int attribute_count;
    struct html_node* children[MAX_CHILDREN];
    int child_count;
    struct html_node* parent;
} html_node_t;

typedef struct {
    html_node_t** nodes;
    int count;
    int capacity;
} html_node_list_t;

typedef struct {
    const char* html;
    size_t length;
    size_t position;
    int line;
    int column;
} html_parser_t;

html_node_t* html_node_create(html_node_type_t type);
void html_node_free(html_node_t* node);
void html_node_add_child(html_node_t* parent, html_node_t* child);
void html_node_add_attribute(html_node_t* node, const char* name, const char* value);
const char* html_node_get_attribute(html_node_t* node, const char* name);

html_parser_t* html_parser_create(const char* html);
void html_parser_free(html_parser_t* parser);
html_node_t* html_parse_document(html_parser_t* parser);
html_node_t* html_parse_element(html_parser_t* parser);
char* html_parse_text(html_parser_t* parser);

void html_parser_skip_whitespace(html_parser_t* parser);
int html_parser_peek_char(html_parser_t* parser);
int html_parser_next_char(html_parser_t* parser);
int html_parser_match_string(html_parser_t* parser, const char* str);

html_node_list_t* html_find_elements_by_tag(html_node_t* root, const char* tag_name);
html_node_list_t* html_find_elements_by_class(html_node_t* root, const char* class_name);
html_node_t* html_find_element_by_id(html_node_t* root, const char* id);
void html_node_list_free(html_node_list_t* list);

void html_print_node(html_node_t* node, int indent);
char* html_node_get_text_content(html_node_t* node);

#endif // HTML_PARSER_H