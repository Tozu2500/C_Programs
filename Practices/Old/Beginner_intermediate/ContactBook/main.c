#include <stdio.h>
#include <string.h>

#define MAX_CONTACTS 50

struct Contact {
    char name[50];
    char phone[20];
    char email[50];
};

void addContact(struct Contact contacts[], int *count) {
    if (*count >= MAX_CONTACTS) {
        printf("Contact list is full!\n");
        return;
    }
    printf("Enter name: ");
    scanf("%s", contacts[*count].name);
    printf("Enter phone: ");
    scanf("%s", contacts[*count].phone);
    printf("Enter email: ");
    scanf("%s", contacts[*count].email);
    (*count)++;
    printf("New contact added successfully!\n");
}

void listContacts(struct Contact contacts[], int count) {
    if (count == 0) {
        printf("No contacts available\n");
        return;
    }
    printf("\n--- Contact List ---\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s | %s | %s\n", i + 1, contacts[i].name,
                contacts[i].phone, contacts[i].email);

    }
}

void searchContact(struct Contact contacts[], int count) {
    char name[50];
    printf("Enter name to search: ");
    scanf("%s", name);
    for (int i = 0; i < count; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            printf("Found: %s | %s | %s\n", contacts[i].name,
                    contacts[i].phone, contacts[i].email);
            return;
        }
    }
    printf("Contact not found.\n");
}

void deleteContact(struct Contact contacts[], int *count) {
    char name[50];
    printf("Enter name to be deleted: ");
    scanf("%s", name);
    for (int i = 0; i < *count; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            for (int j = i; j < *count - 1; j++) {
                contacts[j] = contacts[j + 1];
            }
            (*count)--;
            printf("Contact successfully removed!\n");
            return;
        }
    }
    printf("Contact was not found!\n");
}

int main() {
    struct Contact contacts[MAX_CONTACTS];
    int count = 0;
    int choice = 0;

    while (1) {
        printf("\n --- Contact Book ---\n");
        printf("1. Add Contact\n2. List Contacts\n3. Search Contact\n");
        printf("4. Delete Contact\n 5.Exit\n Choice: ");
        scanf("%d", &choice);

        if (choice == 1) addContact(contacts, &count);
        else if (choice == 2) listContacts(contacts, count);
        else if (choice == 3) searchContact(contacts, count);
        else if (choice == 4) deleteContact(contacts, &count);
        else if (choice == 5) break;
        else printf("Invalid choice, try again!\n");
    }
    return 0;
}