# The project is very buggy for now

I am probably going to make it better in the future. We'll see.


- add fps limiter
- add double buffering
--------------------------------------------------------------------------
possible fixes that might be worth looking for
https://github.com/copilot/c/f010033d-442e-49ba-9925-69f52bc8cabf
--------------------------------------------------------------------------

Common Causes of Screen Flashing
Full Screen Clears: Calling system("clear"), clrscr(), or similar in every loop iteration.
Redrawing Everything Unnecessarily: Redrawing the entire UI even if nothing changed.
Not Using Double Buffering: Updating the screen directly instead of drawing to a buffer and swapping.
No Frame Limiting: Redrawing as fast as possible, overwhelming the terminal.
Not Using Proper Terminal Libraries: Relying on basic I/O instead of curses/ncurses, which are designed for smooth terminal UIs.