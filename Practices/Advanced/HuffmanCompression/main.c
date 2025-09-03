#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 256

typedef struct Node {
    char ch;
    int freq;
    struct Node *left;
    struct Node *right;
} Node;

Node* newNode(char ch, int freq) {
    Node *n = malloc(sizeof(Node));
    n->ch = ch;
    n->freq = freq;
    n->left = NULL;
    n->right = NULL;
    return n;
}

Node* merge(Node *a, Node *b) {
    Node *n = newNode('\0', a->freq + b->freq);
    n->left = a;
    n->right = b;
    return n;
}

void printCodes(Node *root, char *code, int depth) {
    if (!root) {
        return;
    }

    if (root->ch != '\0') {
        code[depth] = '\0';
        printf("%c: %s\n", root->ch, code);
    }
    code[depth] = '0';
    printCodes(root->left, code, depth + 1);
    code[depth] = '1';
    printCodes(root->right, code, depth + 1);
}

int main() {
    char str[100];
    printf("Enter string: ");
    fgets(str, 100, stdin);

    int freq[MAX] = {0};
    for (int i = 0; str[i]; i++) {
        freq[(unsigned char)str[i]]++;
    }

    Node *nodes[MAX];
    int count = 0;
    for (int i = 0; i < MAX; i++) {
        if (freq[i]) {
            nodes[count++] = newNode(i, freq[i]);
        }
    }

    while (count > 1) {
        int min1 = 0;
        int min2 = 1;
        for (int i = 0; i < count; i++) {
            if (nodes[i]->freq < nodes[min1]->freq) {
                min2 = min1;
                min1 = i;
            }
            else if (nodes[i]->freq < nodes[min2]->freq && i != min1) {
                min2 = i;
            }
        }
        Node *merged = merge(nodes[min1], nodes[min2]);
        nodes[min1] = merged;
        nodes[min2] = nodes[count - 1];
        count--;
    }

    char code[100];
    printf("Huffman Codes:\n");
    printCodes(nodes[0], code, 0);

    return 0;
}