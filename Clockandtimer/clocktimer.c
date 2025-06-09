#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

void clearScreen() {
    system("cls");
}

void wait(int milliseconds) {
    Sleep(milliseconds);
}

void showClock() {
    while (1) {
        SYSTEMTIME st;
        GetLocalTime(&st);

        clearScreen();
        printf("--- Clock in your timezone in real time ---\n");
        printf("Current Time is: %02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond);
        printf("\n Press CTRL + C to exit.\n");

        wait(1000);
    }
}

void startTimer(int seconds) {
    printf("--- Countdown timer ---\n");
    while (seconds >= 0) {
        int mins = seconds / 60;
        int secs = seconds % 60;
        printf("\r Time left: %02d:%02d ", mins, secs);
        fflush(stdout);
        wait(1000);
        seconds--;
    }
    printf("\n Time is up! \n");
    Beep(1000, 500);
    Beep(1500, 500);
}

void startStopwatch() {
    int seconds = 0;
    printf("--- Stopwatch ---\n");
    printf("Press CTRL + C to stop.\n");
    while (1) {
        int mins = seconds / 60;
        int secs = seconds % 60;
        printf("\r Elapsed Time: %02d:%02d ", mins, secs);
        fflush(stdout);
        wait(1000);
        seconds++;
    }
}

void setAlarm() {
    int targetHour, targetMinute;
    printf("--- Set alarm ---\n");
    printf("Enter alarm time: (HH : MM) ");
    scanf("%d %d", &targetHour, &targetMinute);
    
    while (1) {
        SYSTEMTIME st;
        GetLocalTime(&st);

        if (st.wHour == targetHour && st.wMinute == targetMinute) {
            printf("\n Alarm time is %02d:%02d", targetHour, targetMinute);
            for (int i = 0; i < 3; i++) {
                Beep(1500, 700);
                wait(300);
            }
            break;
        }

        wait(1000);
    }
}

int main() {
    int choice;

    while(1) {
        clearScreen();
        printf("--- Clock & Timer utility for windows\n");
        printf("1. Show Real-Time Clock\n");
        printf("2. Start countdown timer\n");
        printf("3. Start stopwatch\n");
        printf("4. Set alarm\n");
        printf("5 Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        clearScreen();

        switch(choice) {
            case 1:
                showClock();
                break;
            case 2:
                int sec;
                printf("Enter timer duration in seconds: ");
                scanf("%d", &sec);
                startTimer(sec);
                break;
            case 3:
                startStopwatch();
                break;
            case 4:
                setAlarm();
                break;
            case 5:
                printf("Goodbye!\n");
                exit(0);
            default:
                printf("Please select a valid choice\n");
                wait(2000);
        }
    }
    return 0;
}