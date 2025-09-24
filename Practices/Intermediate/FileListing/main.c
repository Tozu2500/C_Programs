#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define CMD_BUF 256
#define PATH_MAX_LEN 4096

// Print formatted file size
void print_size(off_t size) {
    const char *units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    double sz = size;

    while (sz > 1024 && unit < 3) {
        sz /= 1024;
        unit++;
    }

    printf("%7.2f %-2s", sz, units[unit]);
}

// List files in current directory
void list_files(const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    char fullpath[PATH_MAX_LEN];

    dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    printf("\nListing: %s\n", path);
    printf("%-30s %-10s %-20s\n", "Name", "Size", "Last Modified");
    printf("---------------------------------------------------------------\n");

    while ((entry = readdir(dir)) != NULL) {
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &fileStat) == -1) {
            perror("stat");
            continue;
        }

        char timebuf[64];
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S",
                 localtime(&fileStat.st_mtime));

        printf("%-30s ", entry->d_name);
        print_size(fileStat.st_size);
        printf("   %-20s\n", timebuf);
    }

    closedir(dir);
}

// Run command loop
void cli_loop() {
    char cwd[PATH_MAX_LEN];
    char cmd[CMD_BUF];

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            return;
        }

        printf("\n[%s]$ ", cwd);
        if (!fgets(cmd, sizeof(cmd), stdin)) {
            printf("\n");
            break;
        }

        // remove newline
        cmd[strcspn(cmd, "\n")] = '\0';

        if (strncmp(cmd, "ls", 2) == 0) {
            list_files(cwd);
        } else if (strncmp(cmd, "cd ", 3) == 0) {
            char *dir = cmd + 3;
            if (chdir(dir) != 0) {
                perror("cd");
            }
        } else if (strcmp(cmd, "exit") == 0) {
            printf("Exiting.\n");
            break;
        } else if (strcmp(cmd, "") == 0) {
            continue;
        } else {
            printf("Unknown command: %s\n", cmd);
            printf("Commands: ls, cd <dir>, exit\n");
        }
    }
}

int main() {
    printf("Simple File Explorer CLI\n");
    printf("Commands: ls, cd <dir>, exit\n");

    cli_loop();

    return 0;
}
