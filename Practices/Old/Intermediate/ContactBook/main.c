#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CONTACTS 200
#define NAME_LEN 50
#define PHONE_LEN 15
#define EMAIL_LEN 50

typedef struct {
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    char email[EMAIL_LEN];
} Contact;

Contact contacts[MAX_CONTACTS];
int contactCount = 0;

void addContact() {
    if (contactCount >= MAX_CONTACTS) {
        printf("Your contact book is full!\n");
        return;
    }
    printf("Enter name: ");
    scanf(" %[^\n]", contacts[contactCount].name);
    printf("Enter phone: ");
    scanf(" %[^\n]", contacts[contactCount].phone);
    printf("Enter email: ");
    scanf(" %[^\n]", contacts[contactCount].email);
    contactCount++;
    printf("Contact added!\n");
}

void listContacts() {
    if (contactCount == 0) {
        printf("No contacts found\n");
        return;
    }

    for (int i = 0; i < contactCount; i++) {
        printf("%d. %s | %s | %s\n", i + 1,
            contacts[i].name,
            contacts[i].phone,
            contacts[i].email);
    }
}

void searchContacts() {
    char query[NAME_LEN];
    printf("Enter name to search: ");
    scanf(" %[^\n]", query);

    for (int i = 0; i < contactCount; i++) {
        if (strstr(contacts[i].name, query)) {
            printf("Found: %s | %s | %s\n",
                contacts[i].name,
                contacts[i].phone,
                contacts[i].email);
            return;
        }
    }
    printf("No matches found\n");
}

void deleteContact() {
    int idx;
    listContacts();
    printf("Enter number to delete: ");
    scanf("%d", &idx);
    if (idx < 1 || idx > contactCount) {
        printf("Invalid choice\n");
        return;
    }

    for (int i = idx - 1; i < contactCount - 1; i++) {
        contacts[i] = contacts[i + 1];
    }
    contactCount--;
    printf("Contact deleted!\n");
}

void saveContacts() {
    FILE *fp = fopen("contacts.txt", "w");
    if (!fp) {
        printf("Error saving .txt file\n");
        return;
    }

    for (int i = 0; i < contactCount; i++) {
        fprintf(fp, "%s,%s,%s\n",
            contacts[i].name,
            contacts[i].phone,
            contacts[i].email);
    }
    fclose(fp);
}

void loadContacts() {
    FILE *fp = fopen("contacts.txt", "r");
    if (!fp) return;
    while (fscanf(fp, "%49[^,],%14[^,],%49[^\n]\n",
                contacts[contactCount].name,
                contacts[contactCount].phone,
                contacts[contactCount].email) == 3) {
                    contactCount++;
                }
    fclose(fp);
}

int main() {
    loadContacts();
    int choice = 0;

    do {
        printf("\n--- Contact Book ---\n");
        printf("1. Add Contact\n2. List Contacts\n3. Search Contact\n");
        printf("4. Delete Contact\n5. Save & Exit\nChoice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addContact();
                break;
            case 2:
                listContacts();
                break;
            case 3:
                searchContacts();
                break;
            case 4:
                deleteContact();
                break;
            case 5:
                saveContacts();
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid option...\n");
        }
    } while (choice != 5);
    
    return 0;
}