#include "backup_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int initialize_backup_catalog(BackupCatalog* catalog, const char* path) {
    if (!catalog || !path) return -1;
    
    memset(catalog, 0, sizeof(BackupCatalog));
    strncpy(catalog->catalog_path, path, MAX_PATH_LENGTH - 1);
    catalog->catalog_path[MAX_PATH_LENGTH - 1] = '\0';
    
    return load_backup_catalog(catalog);
}

int save_backup_catalog(const BackupCatalog* catalog) {
    if (!catalog) return -1;
    
    FILE* file = fopen(catalog->catalog_path, "wb");
    if (!file) return -1;
    
    fwrite(&catalog->set_count, sizeof(int), 1, file);
    fwrite(catalog->sets, sizeof(BackupSet), catalog->set_count, file);
    
    fclose(file);
    return 0;
}

int load_backup_catalog(BackupCatalog* catalog) {
    if (!catalog) return -1;
    
    FILE* file = fopen(catalog->catalog_path, "rb");
    if (!file) {
        catalog->set_count = 0;
        return 0;
    }
    
    if (fread(&catalog->set_count, sizeof(int), 1, file) != 1) {
        fclose(file);
        catalog->set_count = 0;
        return -1;
    }
    
    if (catalog->set_count > MAX_BACKUP_SETS) {
        fclose(file);
        catalog->set_count = 0;
        return -1;
    }
    
    if (fread(catalog->sets, sizeof(BackupSet), catalog->set_count, file) != catalog->set_count) {
        fclose(file);
        catalog->set_count = 0;
        return -1;
    }
    
    fclose(file);
    return 0;
}

int add_backup_to_catalog(BackupCatalog* catalog, const BackupSet* set) {
    if (!catalog || !set) return -1;
    
    if (catalog->set_count >= MAX_BACKUP_SETS) return -1;
    
    catalog->sets[catalog->set_count] = *set;
    catalog->set_count++;
    
    return save_backup_catalog(catalog);
}

int create_backup_manifest(const char* backup_path, const char* source_path) {
    if (!backup_path || !source_path) return -1;
    
    char manifest_path[MAX_PATH_LENGTH];
    snprintf(manifest_path, MAX_PATH_LENGTH, "%s\\manifest.dat", backup_path);
    
    FILE* manifest = fopen(manifest_path, "wb");
    if (!manifest) return -1;
    
    int entry_count = 0;
    fwrite(&entry_count, sizeof(int), 1, manifest);
    
    char search_path[MAX_PATH_LENGTH];
    snprintf(search_path, MAX_PATH_LENGTH, "%s\\*", source_path);
    
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(search_path, &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(findData.cFileName, ".") == 0 || 
                strcmp(findData.cFileName, "..") == 0) {
                continue;
            }
            
            BackupFileEntry entry;
            memset(&entry, 0, sizeof(entry));
            
            strncpy(entry.filename, findData.cFileName, MAX_PATH - 1);
            entry.filename[MAX_PATH - 1] = '\0';
            
            LARGE_INTEGER size;
            size.HighPart = findData.nFileSizeHigh;
            size.LowPart = findData.nFileSizeLow;
            entry.size = size.QuadPart;
            
            entry.modified_time = findData.ftLastWriteTime;
            entry.attributes = findData.dwFileAttributes;
            
            char full_path[MAX_PATH_LENGTH];
            snprintf(full_path, MAX_PATH_LENGTH, "%s\\%s", source_path, findData.cFileName);
            
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                calculate_file_hash(full_path, entry.hash, 32);
            }
            
            fwrite(&entry, sizeof(BackupFileEntry), 1, manifest);
            entry_count++;
            
        } while (FindNextFileA(hFind, &findData));
        
        FindClose(hFind);
    }
    
    fseek(manifest, 0, SEEK_SET);
    fwrite(&entry_count, sizeof(int), 1, manifest);
    
    fclose(manifest);
    return 0;
}

int load_backup_manifest(const char* backup_path, BackupFileEntry** entries, int* count) {
    if (!backup_path || !entries || !count) return -1;
    
    char manifest_path[MAX_PATH_LENGTH];
    snprintf(manifest_path, MAX_PATH_LENGTH, "%s\\manifest.dat", backup_path);
    
    FILE* manifest = fopen(manifest_path, "rb");
    if (!manifest) return -1;
    
    if (fread(count, sizeof(int), 1, manifest) != 1) {
        fclose(manifest);
        return -1;
    }
    
    *entries = (BackupFileEntry*)malloc(sizeof(BackupFileEntry) * (*count));
    if (!*entries) {
        fclose(manifest);
        return -1;
    }
    
    if (fread(*entries, sizeof(BackupFileEntry), *count, manifest) != *count) {
        free(*entries);
        *entries = NULL;
        fclose(manifest);
        return -1;
    }
    
    fclose(manifest);
    return 0;
}

int create_backup(const char* source, const char* dest, BackupType type, const CopyOptions* options) {
    if (!source || !dest || !options) return -1;
    
    char backup_dir[MAX_PATH_LENGTH];
    SYSTEMTIME st;
    GetLocalTime(&st);
    
    snprintf(backup_dir, MAX_PATH_LENGTH, "%s\\backup_%04d%02d%02d_%02d%02d%02d",
             dest, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    
    CopyOptions backup_opts = *options;
    strncpy(backup_opts.source, source, MAX_PATH_LENGTH - 1);
    strncpy(backup_opts.destination, backup_dir, MAX_PATH_LENGTH - 1);
    
    CopyStatistics stats;
    int result = perform_copy_operation(&backup_opts, &stats);
    
    if (result == 0) {
        create_backup_manifest(backup_dir, source);
    }
    
    return result;
}

int restore_backup(const char* backup_path, const char* restore_path, const CopyOptions* options) {
    if (!backup_path || !restore_path || !options) return -1;
    
    CopyOptions restore_opts = *options;
    strncpy(restore_opts.source, backup_path, MAX_PATH_LENGTH - 1);
    strncpy(restore_opts.destination, restore_path, MAX_PATH_LENGTH - 1);
    
    CopyStatistics stats;
    return perform_copy_operation(&restore_opts, &stats);
}

int verify_backup_integrity(const BackupSet* set) {
    if (!set) return -1;
    
    BackupFileEntry* entries = NULL;
    int count = 0;
    
    if (load_backup_manifest(set->backup_path, &entries, &count) != 0) {
        return -1;
    }
    
    int verified = 0;
    for (int i = 0; i < count; i++) {
        char file_path[MAX_PATH_LENGTH];
        snprintf(file_path, MAX_PATH_LENGTH, "%s\\%s", 
                set->backup_path, entries[i].filename);
        
        FileInfo info;
        if (get_file_info(file_path, &info) == 0) {
            if (info.size == entries[i].size) {
                unsigned char hash[32];
                if (calculate_file_hash(file_path, hash, 32) == 0) {
                    if (memcmp(hash, entries[i].hash, 32) == 0) {
                        verified++;
                    }
                }
            }
        }
    }
    
    free(entries);
    return (verified == count) ? 0 : -1;
}

int list_backups(const BackupCatalog* catalog) {
    if (!catalog) return -1;
    
    printf("\n=== Backup Catalog ===\n");
    printf("Total backups: %d\n\n", catalog->set_count);
    
    for (int i = 0; i < catalog->set_count; i++) {
        const BackupSet* set = &catalog->sets[i];
        printf("Backup #%d:\n", i + 1);
        printf("  Name: %s\n", set->name);
        printf("  Type: %s\n", 
               set->type == BACKUP_TYPE_FULL ? "Full" :
               set->type == BACKUP_TYPE_INCREMENTAL ? "Incremental" : "Differential");
        printf("  Source: %s\n", set->source_path);
        printf("  Backup: %s\n", set->backup_path);
        printf("  Files: %llu\n", set->file_count);
        printf("  Size: %llu bytes\n", set->total_size);
        printf("  Valid: %s\n\n", set->is_valid ? "Yes" : "No");
    }
    
    return 0;
}

int delete_backup(BackupCatalog* catalog, const char* backup_name) {
    if (!catalog || !backup_name) return -1;
    
    int found_index = -1;
    for (int i = 0; i < catalog->set_count; i++) {
        if (strcmp(catalog->sets[i].name, backup_name) == 0) {
            found_index = i;
            break;
        }
    }
    
    if (found_index == -1) return -1;
    
    for (int i = found_index; i < catalog->set_count - 1; i++) {
        catalog->sets[i] = catalog->sets[i + 1];
    }
    
    catalog->set_count--;
    
    return save_backup_catalog(catalog);
}