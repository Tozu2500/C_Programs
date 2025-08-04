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
}