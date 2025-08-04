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
