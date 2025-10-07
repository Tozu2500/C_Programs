#ifndef SYSMON_H
#define SYSMON_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <tlhelp32.h>
#include <psapi.h>

#define MAX_PROCESSES 256
#define MAX_NAME_LEN 256
#define MAX_PATH_LEN 512
#define UPDATE_INTERVAL 1000
#define PROGRESS_BAR_WIDTH 50

#pragma comment(lib, "psapi.lib")

#endif // SYSMON_H