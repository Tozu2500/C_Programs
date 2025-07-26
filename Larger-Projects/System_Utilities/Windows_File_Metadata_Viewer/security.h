#ifndef SECURITY_H
#define SECURITY_H

#include <windows.h>
#include "file_utils.h"

typedef struct {
    char owner[256];
    char group[256];
    DWORD access_rights;
    int can_read;
    int can_write;
    int can_execute;
    int can_delete;
} SecurityInfo;

int get_file_security_info(const char* filepath, SecurityInfo* security);
void display_security_info(const SecurityInfo* security);
int check_file_access(const char* filepath, DWORD desired_access);
int is_file_safe_to_access(const char* filepath);
void display_permissions_summary(const SecurityInfo* security);

#endif // SECURITY_H