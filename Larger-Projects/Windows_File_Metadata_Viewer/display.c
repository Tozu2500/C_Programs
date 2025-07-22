#include "display.h"
#include <stdio.h>
#include <stdlib.h>

void display_banner(void) {
    printf("============================================\n");
    printf("        Windows File Metadata Viewer        \n");
    printf("        Advanced File Analysis Tool         \n");
    printf("============================================\n");
}

void display_file_metadata(const FileInfo* info) {
    char size_str[64];
    char type_desc[128];

    format_file_size(info->size_high, info->size_low, size_str, sizeof(size_str));
    strcpy_s(type_desc, sizeof(type_desc), get_file_type_description(info->extension));

    printf("File Information\n");
    printf("---------------------------------------");
    printf("Name:       %s\n", info->name);
    printf("Extension:  %s\n", strlen(info->extension) > 0 ? info->extension : "None");
    printf("Type:       %s\n", type_desc);
    printf("Size:       %s\n", size_str);
    printf("Directory:  %s\n", info->is_directory ? "Yes" : "No");
    printf("\n");
}

void display_attributes_table(const FileInfo* info) {
    printf("File Attributes\n");
    printf("---------------------------------------");
    printf("Hidden:         %s\n", info->is_hidden ? "Yes" : "No");
    printf("Read-only:      %s\n", info->is_readonly ? "Yes" : "No");
    printf("System:         %s\n", info->is_system ? "Yes" : "No");
    printf("Archive:        %s\n", (info->attributes & FILE_ATTRIBUTE_ARCHIVE) ? "Yes" : "No");
    printf("Compressed:     %s\n", (info->attributes & FILE_ATTRIBUTE_COMPRESSED) ? "Yes" : "No");
    printf("Encrypted:      %s\n", (info->attributes & FILE_ATTRIBUTE_ENCRYPTED) ? "Yes" : "No");
    printf("Temporary:      %s\n", (info->attributes & FILE_ATTRIBUTE_TEMPORARY) ? "Yes" : "No");
    printf("Sparse:         %s\n", (info->attributes & FILE_ATTRIBUTE_SPARSE_FILE) ? "Yes" : "No");
    printf("\n");
}

void display_size_analysis(const FileInfo* info) {
    if (info->is_directory) {
        printf("Size analysis\n");
        printf("---------------------------------------");
        printf("This is a directory - size analysis not applicable.\n\n");
        return;
    }

    ULARGE_INTEGER file_size;
    file_size.HighPart = info->size_high;
    file_size.LowPart = info->size_low;

    printf("Size Analysis\n");
    printf("---------------------------------------");
    printf("Exact bytes:       %llu\n", file_size.QuadPart);
    printf("Kilobytes (KB):    %.2f KB\n", (double)file_size.QuadPart / 1024.0);
    printf("Megabytes (MB):    %.4f MB\n", (double)file_size.QuadPart / (1024.0 * 1024.0));
    printf("Gigabytes (GB):    %.6f GB\n", (double)file_size.QuadPart / (1024.0 * 1024.0 * 1024.0));

    if (file_size.QuadPart == 0) {
        printf("Status:         Empty file\n");
    } else if (file_size.QuadPart < 1024) {
        printf("Status:         Less than a KB\n");
    } else if (file_size.QuadPart < 1024 * 1024) {
        printf("Status:         Small file (MBs)");
    } else if (file_size.QuadPart < 1024 * 1024 * 100) {
        printf("Status:         Medium file (GBs)");
    } else {
        printf("Status          Large file (TBs or GBs)");
    }
    printf("\n");
}

void display_time_information(const FileInfo* info) {
    char creation_str[32];
    char modified_str[32];
    char access_str[32];

    format_file_time((FILETIME*)&info->creation_time, creation_str, sizeof(creation_str));
    format_file_time((FILETIME*)&info->modified_time, modified_str, sizeof(modified_str));
    format_file_time((FILETIME*)&info->access_time, access_str, sizeof(access_str));

    printf("Time Information\n");
    printf("---------------------------------------");
    printf("Created:        %s\n", creation_str);
    printf("Modified:       %s\n", modified_str);
    printf("Last Access     %s\n", access_str);
    printf("\n");
}

void display_error(const char* message) {
    printf("ERROR: %s\n", message);
}

void display_help(void) {
    printf("USAGE:\n");
    printf("------\n");
    printf("file_metadata_viewer.exe (Run the program with the filepath) <filepath>\n");
    printf("\nEXAMPLES\n");
    printf("--------\n");
    printf("file_metadata_viewer C:\\Windows\\System32\\notepad.exe\n");
    printf("file_metadata_viewer \'c:\\My Documents\\report.pdf\"\n");
    printf("file_metadata_viewer.exe .\n");
    printf("FEATURES\n");
    printf("--------\n");
    printf("- Comprehensive file information display\n");
    printf("- Detailed size analysis with multiple units\n");
    printf("- Complete timestamp information\n");
    printf("- File attribute detection\n");
    printf("- File type identification\n");
    printf("- Support for files and directories\n");
    printf("\n");
}

void clear_screen(void) {
    system("cls");
}

void pause_for_input(void) {
    printf("Press any key to continue...");
    getchar();
}