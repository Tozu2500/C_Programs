#include "file_utils.h"
#include <string.h>

int file_exists(const char* filepath) {
    DWORD attributes = GetFileAttributesA(filepath);
    return (attributes != INVALID_FILE_ATTRIBUTES);
}

int get_file_info(const char* filepath, FileInfo* info) {
    WIN32_FIND_DATAA find_data;
    HANDLE hFind = FindFirstFileA(filepath, &find_data);

    if (hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }

    strcpy_s(info->name, MAX_PATH, find_data.cFileName);

    char* ext = strrchr(find_data.cFileName, '.');
    if (ext) {
        strcpy_s(info->extension, 32, ext + 1);
    } else {
        strcpy_s(info->extension, 32, "");
    }

    info->size_high = find_data.nFileSizeHigh;
    info->size_low = find_data.nFileSizeLow;
    info->creation_time = find_data.ftCreationTime;
    info->modified_time = find_data.ftLastWriteTime;
    info->access_time = find_data.ftLastAccessTime;
    info->attributes = find_data.dwFileAttributes;

    info->is_directory = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    info->is_hidden = (find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
    info->is_readonly = (find_data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
    info->is_system = (find_data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0;

    FindClose(hFind);
    return 1;
}

void format_file_size(DWORD size_high, DWORD size_low, char* buffer, int buffer_size) {
    ULARGE_INTEGER file_size;
    file_size.HighPart = size_high;
    file_size.LowPart = size_low;

    double size = (double)file_size.QuadPart;
    const char* units[] = {"bytes", "KB", "MB", "GB", "TB"};
    int unit_index = 0;

    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }

    if (unit_index == 0) {
        sprintf_s(buffer, buffer_size, "%.0f %s", size, units[unit_index]);
    } else {
        sprintf_s(buffer, buffer_size, "%.2f %s", size, units[unit_index]);
    }
}

void format_file_time(FILETIME* ft, char* buffer, int buffer_size) {
    SYSTEMTIME st;
    FileTimeToSystemTime(ft, &st);
    sprintf_s(buffer, buffer_size, "%04d-%02d-%02d %02d:%02d:%02d",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

const char* get_file_type_description(const char* extension) {
    if (strlen(extension) == 0) return "File";
    if (strcmp(extension, "txt") == 0) return "Text Document";
    if (strcmp(extension, "doc") == 0) return "Word Document";
    if (strcmp(extension, "docx") == 0) return "Word Document";
    if (strcmp(extension, "pdf") == 0) return "PDF Document";
    if (strcmp(extension, "jpg") == 0) return "JPEG Image";
    if (strcmp(extension, "jpeg") == 0) return "JPEG Image";
    if (strcmp(extension, "png") == 0) return "PNG Image";
    if (strcmp(extension, "gif") == 0) return "GIF Image";
    if (strcmp(extension, "bmp") == 0) return "Bitmap Image";
    if (strcmp(extension, "mp3") == 0) return "MP3 Audio";
    if (strcmp(extension, "wav") == 0) return "WAV Audio";
    if (strcmp(extension, "mp4") == 0) return "MP4 Video";
    if (strcmp(extension, "avi") == 0) return "AVI Video";
    if (strcmp(extension, "exe") == 0) return "Executable";
    if (strcmp(extension, "dll") == 0) return "Dynamic Link Library (dll)";
    if (strcmp(extension, "zip") == 0) return "ZIP Archive";
    if (strcmp(extension, "rar") == 0) return "RAR Archive";
    if (strcmp(extension, "c") == 0) return "C Source Code";
    if (strcmp(extension, "cpp") == 0) return "C++ Source Code";
    if (strcmp(extension, "h") == 0) return "C/C++ Header File";
    if (strcmp(extension, "py") == 0) return "Python Script";
    if (strcmp(extension, "java") == 0) return "Java Source Code";
    if (strcmp(extension, "html") == 0) return "HTML Document";
    if (strcmp(extension, "css") == 0) return "CSS Stylesheet";
    if (strcmp(extension, "js") == 0) return "JavaScript File";
    return "Unknown File Type";
}
