#include <stdio.h>
#include <string.h>
#include "editor.h"

int main(int argc, char **argv) {
    EditorState E;
    editorInit(&E);

    if (argc >= 2) {
        editorOpen(&E, argv[1]);
    }

    while (1) {
        editorRefreshScreen(&E);
        editorProcessKeypress(&E);
    }

    editorFree(&E);

    return 0;
}