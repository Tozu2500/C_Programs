#ifndef INPUT_H
#define INPUT_H

#include "types.h"
#include "console.h"

int handleInput(Console* console, FileList* fileList, int key);
void navigateUp(FileList* fileList);
void navigateDown(FileList* fileList);
int openSelected(FileList* fileList);
int goBack(FileList* fileList);

#endif // INPUT_H