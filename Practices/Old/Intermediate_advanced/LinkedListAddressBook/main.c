#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char name[50];
    char phone[20];
    struct Node *next;
} Node;

Node* addContact(Node *head, char *name, char *phone) {
    Node *newNode = malloc(sizeof(Node));
    strcpy(newNode->name, name);
    strcpy(newNode->phone, phone);
    newNode->next = head;
    return newNode;
}

Node* deleteContact(Node *head, char *name) {
    Node *curr = head, *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr);
            return head;
        }
        prev = curr; curr = curr->next;
    }
    return head;
}

void listContacts(Node *head) {
    while (head) {
        printf("%s - %s\n", head->name, head->phone);
        head = head->next;
    }
}

int main() {
    Node *head = NULL;
    int choice;
    char name[50];
    char phone[20];
    
    while (1) {
        printf("\n1. Add\n2. Delete\n3. List\n4. Exit\nChoice: ");
        scanf("%d", &choice);
        if (choice == 1) {
            printf("Enter name: ");
            scanf("%s", name);
            printf("Enter phone: ");
            scanf("%s", phone);
            head = addContact(head, name, phone);
        } else if (choice == 2) {
            printf("Enter name: ");
            scanf("%s", name);
            head = deleteContact(head, name);
        } else if (choice == 3) listContacts(head);
        else break;
    }
    return 0;
}