#include "file_operations.h"
#include "backup_manager.h"
#include "logger.h"
#include "threading.h"
#include "ui_interface.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

static Logger global_logger;
static BackupCatalog global_catalog;
static CopyOptions global_options;
static UIConfig ui_config;

// Progress display helper function for copy operations
static void display_copy_progress(const CopyStatistics* stats) {
    if (!stats) return;
    
    double file_progress = (stats->total_files > 0) ? 
        (100.0 * stats->copied_files / stats->total_files) : 0.0;
    double byte_progress = 0.0;
    
    if (stats->total_bytes > 0) {
        byte_progress = 100.0 * stats->copied_bytes / stats->total_bytes;
    }
    
    double total_mb = stats->total_bytes / (1024.0 * 1024.0);
    double copied_mb = stats->copied_bytes / (1024.0 * 1024.0);
    
    // Clear current line and display progress
    printf("\r  Progress: %llu/%llu files (%.1f%%) | %.2f MB / %.2f MB (%.1f%%)          ",
           stats->copied_files, stats->total_files, file_progress,
           copied_mb, total_mb, byte_progress);
    fflush(stdout);
    
    // If complete, add newline
    if (stats->copied_files >= stats->total_files) {
        printf("\n");
    }
}

int handle_copy_operation(void) {
    char source[MAX_PATH_LENGTH];
    char dest[MAX_PATH_LENGTH];
    
    printf("\n");
    get_user_input("  Enter source path: ", source, sizeof(source));
    get_user_input("  Enter destination path: ", dest, sizeof(dest));
    
    strncpy(global_options.source, source, MAX_PATH_LENGTH - 1);
    strncpy(global_options.destination, dest, MAX_PATH_LENGTH - 1);
    global_options.source[MAX_PATH_LENGTH - 1] = '\0';
    global_options.destination[MAX_PATH_LENGTH - 1] = '\0';
    
    if (!confirm_action("\n  Proceed with copy operation?")) {
        printf("\n  Operation cancelled.\n");
        return 0;
    }
    
    printf("\n  Starting copy operation...\n");
    printf("  [!] Please do not type during the operation.\n");
    
    // Flush stdin
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    CopyStatistics stats;
    memset(&stats, 0, sizeof(CopyStatistics));
    
    // Set progress callback
    stats.progress_callback = display_copy_progress;
    
    // Perform the copy operation (uses single-threaded for reliability)
    int result = perform_copy_operation(&global_options, &stats);
    
    if (result != 0) {
        printf("\n");
        display_error("Copy operation failed!");
        
        // Flush any remaining input
        while ((c = getchar()) != '\n' && c != EOF);
        return -1;
    }
    
    printf("\n");
    display_statistics(&stats);
    log_statistics(&global_logger, &stats);
    
    printf("\n");
    display_success("Copy operation completed!");
    
    // Flush any remaining input
    while ((c = getchar()) != '\n' && c != EOF);
    
    return 0;
}

int handle_create_backup(void) {
    char source[MAX_PATH_LENGTH];
    char dest[MAX_PATH_LENGTH];
    char backup_name[MAX_BACKUP_NAME];
    int type_choice;
    
    printf("\n");
    get_user_input("  Enter source path: ", source, sizeof(source));
    get_user_input("  Enter backup destination: ", dest, sizeof(dest));
    get_user_input("  Enter backup name: ", backup_name, sizeof(backup_name));
    
    printf("\n  Backup Type:\n");
    printf("    [1] Full Backup\n");
    printf("    [2] Incremental Backup\n");
    printf("    [3] Differential Backup\n");
    type_choice = get_user_choice(1, 3);
    
    BackupType type = (BackupType)(type_choice - 1);
    
    if (!confirm_action("\n  Proceed with backup creation?")) {
        printf("\n  Operation cancelled.\n");
        return 0;
    }
    
    printf("\n  Creating backup '%s'...\n", backup_name);
    printf("  [!] Please do not type during the backup operation.\n\n");
    
    // Flush stdin to prevent accidental key presses from affecting the operation
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    log_message(&global_logger, LOG_LEVEL_INFO, "Starting backup: %s", backup_name);
    
    int result = create_backup(source, dest, type, &global_options);
    
    if (result == 0) {
        BackupSet new_set;
        memset(&new_set, 0, sizeof(BackupSet));
        
        strncpy(new_set.name, backup_name, MAX_BACKUP_NAME - 1);
        strncpy(new_set.source_path, source, MAX_PATH_LENGTH - 1);
        strncpy(new_set.backup_path, dest, MAX_PATH_LENGTH - 1);
        new_set.type = type;
        GetSystemTimeAsFileTime(&new_set.created_time);
        new_set.is_valid = true;
        
        add_backup_to_catalog(&global_catalog, &new_set);
        
        printf("\n");
        display_success("Backup created successfully!");
        log_message(&global_logger, LOG_LEVEL_INFO, "Backup completed: %s", backup_name);
    } else {
        printf("\n");
        display_error("Backup creation failed!");
        log_message(&global_logger, LOG_LEVEL_ERROR, "Backup failed: %s", backup_name);
    }
    
    // Flush any remaining input
    while ((c = getchar()) != '\n' && c != EOF);
    
    return result;
}

int handle_restore_backup(void) {
    if (global_catalog.set_count == 0) {
        display_error("No backups available");
        return -1;
    }
    
    printf("\n");
    display_backup_list(&global_catalog);
    
    printf("  Enter backup number to restore: ");
    int choice = get_user_choice(1, global_catalog.set_count);
    
    char restore_path[MAX_PATH_LENGTH];
    get_user_input("  Enter restore destination: ", restore_path, sizeof(restore_path));
    
    if (!confirm_action("\n  Proceed with restore operation?")) {
        printf("\n  Operation cancelled.\n");
        return 0;
    }
    
    printf("\n  Restoring backup...\n\n");
    
    BackupSet* selected = &global_catalog.sets[choice - 1];
    
    log_message(&global_logger, LOG_LEVEL_INFO, "Starting restore: %s", selected->name);
    
    int result = restore_backup(selected->backup_path, restore_path, &global_options);
    
    if (result == 0) {
        display_success("Backup restored successfully");
        log_message(&global_logger, LOG_LEVEL_INFO, "Restore completed: %s", selected->name);
    } else {
        display_error("Backup restore failed");
        log_message(&global_logger, LOG_LEVEL_ERROR, "Restore failed: %s", selected->name);
    }
    
    return result;
}

int handle_list_backups(void) {
    printf("\n");
    display_backup_list(&global_catalog);
    
    printf("\n  Press any key to continue...");
    _getch();
    
    return 0;
}

int handle_verify_backup(void) {
    if (global_catalog.set_count == 0) {
        display_error("No backups available");
        return -1;
    }
    
    printf("\n");
    display_backup_list(&global_catalog);
    
    printf("  Enter backup number to verify: ");
    int choice = get_user_choice(1, global_catalog.set_count);
    
    BackupSet* selected = &global_catalog.sets[choice - 1];
    
    printf("\n  Verifying backup integrity...\n");
    
    log_message(&global_logger, LOG_LEVEL_INFO, "Verifying backup: %s", selected->name);
    
    int result = verify_backup_integrity(selected);
    
    if (result == 0) {
        selected->is_valid = true;
        display_success("Backup verification passed");
        log_message(&global_logger, LOG_LEVEL_INFO, "Verification passed: %s", selected->name);
    } else {
        selected->is_valid = false;
        display_error("Backup verification failed");
        log_message(&global_logger, LOG_LEVEL_ERROR, "Verification failed: %s", selected->name);
    }
    
    save_backup_catalog(&global_catalog);
    
    printf("\n  Press any key to continue...");
    _getch();
    
    return result;
}

int handle_delete_backup(void) {
    if (global_catalog.set_count == 0) {
        display_error("No backups available");
        return -1;
    }
    
    printf("\n");
    display_backup_list(&global_catalog);
    
    printf("  Enter backup number to delete: ");
    int choice = get_user_choice(1, global_catalog.set_count);
    
    BackupSet* selected = &global_catalog.sets[choice - 1];
    
    char confirm_msg[256];
    snprintf(confirm_msg, sizeof(confirm_msg), 
             "\n  Delete backup '%s'?", selected->name);
    
    if (!confirm_action(confirm_msg)) {
        printf("\n  Operation cancelled.\n");
        return 0;
    }
    
    log_message(&global_logger, LOG_LEVEL_INFO, "Deleting backup: %s", selected->name);
    
    int result = delete_backup(&global_catalog, selected->name);
    
    if (result == 0) {
        display_success("Backup deleted successfully");
        log_message(&global_logger, LOG_LEVEL_INFO, "Backup deleted: %s", selected->name);
    } else {
        display_error("Failed to delete backup");
        log_message(&global_logger, LOG_LEVEL_ERROR, "Delete failed: %s", selected->name);
    }
    
    printf("\n  Press any key to continue...");
    _getch();
    
    return result;
}

int handle_configure_options(void) {
    configure_copy_options_interactive(&global_options);
    
    printf("\n  Press any key to continue...");
    _getch();
    
    return 0;
}

int initialize_application(void) {
    if (init_ui(&ui_config) != 0) {
        return -1;
    }
    
    if (init_logger(&global_logger, "file_copier.log", LOG_LEVEL_INFO) != 0) {
        return -1;
    }
    
    if (initialize_copy_options(&global_options) != 0) {
        cleanup_logger(&global_logger);
        return -1;
    }
    
    if (initialize_backup_catalog(&global_catalog, "backup_catalog.dat") != 0) {
        cleanup_logger(&global_logger);
        return -1;
    }
    
    log_message(&global_logger, LOG_LEVEL_INFO, "Application started");
    
    return 0;
}

int cleanup_application(void) {
    log_message(&global_logger, LOG_LEVEL_INFO, "Application shutting down");
    
    save_backup_catalog(&global_catalog);
    cleanup_logger(&global_logger);
    
    return 0;
}

int main(int argc, char* argv[]) {
    if (initialize_application() != 0) {
        printf("Failed to initialize application\n");
        return 1;
    }
    
    if (argc > 1) {
        if (strcmp(argv[1], "--copy") == 0 && argc >= 4) {
            strncpy(global_options.source, argv[2], MAX_PATH_LENGTH - 1);
            strncpy(global_options.destination, argv[3], MAX_PATH_LENGTH - 1);
            
            CopyStatistics stats;
            int result = perform_copy_operation(&global_options, &stats);
            
            if (result == 0) {
                printf("Copy completed successfully\n");
                printf("Files copied: %llu\n", stats.copied_files);
                printf("Bytes copied: %llu\n", stats.copied_bytes);
            } else {
                printf("Copy operation failed\n");
            }
            
            cleanup_application();
            return result;
        }
        
        if (strcmp(argv[1], "--backup") == 0 && argc >= 4) {
            int result = create_backup(argv[2], argv[3], BACKUP_TYPE_FULL, &global_options);
            
            if (result == 0) {
                printf("Backup created successfully\n");
            } else {
                printf("Backup creation failed\n");
            }
            
            cleanup_application();
            return result;
        }
        
        printf("Usage:\n");
        printf("  %s                           - Interactive mode\n", argv[0]);
        printf("  %s --copy <source> <dest>    - Copy files\n", argv[0]);
        printf("  %s --backup <source> <dest>  - Create backup\n", argv[0]);
        
        cleanup_application();
        return 0;
    }
    
    int running = 1;
    
    while (running) {
        display_menu();
        
        int choice = get_user_choice(1, 8);
        
        switch (choice) {
            case 1:
                handle_copy_operation();
                printf("\n  Press any key to continue...");
                _getch();
                break;
                
            case 2:
                handle_create_backup();
                printf("\n  Press any key to continue...");
                _getch();
                break;
                
            case 3:
                handle_restore_backup();
                printf("\n  Press any key to continue...");
                _getch();
                break;
                
            case 4:
                handle_list_backups();
                break;
                
            case 5:
                handle_verify_backup();
                break;
                
            case 6:
                handle_delete_backup();
                break;
                
            case 7:
                handle_configure_options();
                break;
                
            case 8:
                if (confirm_action("\n  Are you sure you want to exit?")) {
                    running = 0;
                }
                break;
        }
    }
    
    printf("\n  Shutting down...\n");
    cleanup_application();
    
    return 0;
}