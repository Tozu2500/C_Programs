#ifndef BACKUP_MANAGER_H
#define BACKUP_MANAGER_H

#include "file_operations.h"
#include <time.h>

#define MAX_BACKUP_NAME 256
#define MAX_BACKUP_SETS 100

typedef enum {
    BACKUP_TYPE_FULL,
    BACKUP_TYPE_INCREMENTAL,
    BACKUP_TYPE_DIFFERENTIAL
} BackupType;

typedef struct {
    char name[MAX_BACKUP_NAME];
    char source_path[MAX_PATH_LENGTH];
    char backup_path[MAX_PATH_LENGTH];
    BackupType type;
    FILETIME created_time;
    unsigned long long total_size;
    unsigned long long file_count;
    bool is_valid;
} BackupSet;

typedef struct {
    BackupSet sets[MAX_BACKUP_SETS];
    int set_count;
    char catalog_path[MAX_PATH_LENGTH];
} BackupCatalog;

typedef struct {
    char filename[MAX_PATH];
    FILETIME modified_time;
    unsigned long long size;
    DWORD attributes;
    unsigned char hash[32];
} BackupFileEntry;

int create_backup(const char* source, const char* dest, BackupType type, const CopyOptions* options);
int restore_backup(const char* backup_path, const char* restore_path, const CopyOptions* options);
int initialize_backup_catalog(BackupCatalog* catalog, const char* path);
int add_backup_to_catalog(BackupCatalog* catalog, const BackupSet* set);
int save_backup_catalog(const BackupCatalog* catalog);
int load_backup_catalog(BackupCatalog* catalog);
int list_backups(const BackupCatalog* catalog);
int delete_backup(BackupCatalog* catalog, const char* backup_name);
int verify_backup_integrity(const BackupSet* set);
int create_backup_manifest(const char* backup_path, const char* source_path);
int load_backup_manifest(const char* backup_path, BackupFileEntry** entries, int* count);

#endif // BACKUP_MANAGER_H