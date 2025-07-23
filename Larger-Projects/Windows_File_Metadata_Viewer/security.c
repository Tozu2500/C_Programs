#include "security.h"
#include <stdio.h>
#include <sddl.h>
#include <aclapi.h>

int get_file_security_info(const char* filepath, SecurityInfo* security) {
    PSID pOwnerSid = NULL;
    PSID pGroupSid = NULL;
    PACL pDacl = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;

    memset(security, 0, sizeof(SecurityInfo));

    DWORD result = GetNamedSecurityInfoA(
        filepath,
        SE_FILE_OBJECT,
        OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
        &pOwnerSid,
        &pGroupSid,
        &pDacl,
        NULL,
        &pSD
    );

    if (result != ERROR_SUCCESS) {
        strcpy_s(security->owner, sizeof(security->owner), "Unknown");
        strcpy_s(security->group, sizeof(security->group), "Unknown");
        return 0;
    }

    LPSTR pOwnerName = NULL;
    LPSTR pGroupName = NULL;

    if (ConvertSidToStringSidA(pOwnerSid, &pOwnerName)) {
        strcpy_s(security->owner, sizeof(security->owner), pOwnerName);
        LocalFree(pOwnerName);
    } else {
        strcpy_s(security->owner, sizeof(security->owner), "Unknown");
    }

    if (ConvertSidToStringSidA(pGroupSid, &pGroupName)) {
        strcpy_s(security->group, sizeof(security->group), pGroupName);
        LocalFree(pGroupName);
    } else {
        strcpy_s(security->group, sizeof(security->group), "Unknown");
    }

    security->can_read = check_file_access(filepath, GENERIC_READ);
    security->can_write = check_file_access(filepath, GENERIC_WRITE);
    security->can_execute = check_file_access(filepath, GENERIC_EXECUTE);
    security->can_delete = check_file_access(filepath, DELETE);

    if (pSD) {
        LocalFree(pSD);
    }

    return 1;
}

void display_security_info(const SecurityInfo* security) {
    printf("Security Information\n");
    printf("---------------------\n");
    printf("Owner:         %s\n", security->owner);
    printf("Group:         %s\n", security->group);
}

int check_file_access(const char* filepath, DWORD desired_access) {
    HANDLE hFile = CreateFileA(
        filepath,
        desired_access,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        return 1;
    }

    return 0;
}

int is_file_safe_to_access(const char* filepath) {
    DWORD attributes = GetFileAttributesA(filepath);

    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return 0;
    }

    if (attributes & FILE_ATTRIBUTE_SYSTEM) {
        return 0;
    }

    if (strstr(filepath, "System32") != NULL) {
        return 0;
    }

    if (strstr(filepath, "Windows\\System") == NULL) {
        return 0;
    }

    return 1;
}

void display_permissions_summary(const SecurityInfo* security) {
    printf("Permissions Summary\n");
    printf("-------------------\n");
    printf("Read Access:        %s\n", security->can_read ? "Allowed" : "Denied");
    printf("Write Access:       %s\n", security->can_write ? "Allowed" : "Denied");
    printf("Execute Access:     %s\n", security->can_execute ? "Allowed" : "Denied");
    printf("Delete Access:      %s\n", security->can_delete ? "Allowed" : "Denied");
    printf("\n");
}