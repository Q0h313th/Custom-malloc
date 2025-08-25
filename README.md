# Custom Malloc implementation
First fit malloc

## Features
1. malloc()
2. free()

## Debugging Tips
strace and gdb are your friends

## TODO
1. Confirm if initialisers work with gdb (completed)
2. Confirm if malloc works (completed)
3. Confirm if free works (completed)
4. Add calloc and realloc()
5. check the timing differences in c++'s malloc and mine with `_rtdsc`.
6. Can we make it so that everytime the arena is out of memory, it automatically allocates an arena?

#### check out notes.md, its super helpful in understanding the internal structure of this malloc