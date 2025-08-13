# Custom malloc for DSA mini project
    - do we want to do an entire memory allocator?
    - Specifically for the use case of allocating multiple smaller chunks. 
    - free list that holds memory.
    - malloc arena
    - first fit allocator to decrease complexity
    - c++  
    - multiple free lists for each memory block?

## Game Plan:
    - We'll have an arena (1Mb size), which we'll allocate using `mmap()`.
    - Inside it, we'll have bins of different sizes.
    - Each bin holds slabs of specific sizes (mostly 4096 bytes) connected through a linked list
    - we have a struct slab. This struct contains a linked list of blocks.
    - First we segment the arena into slabs.
    - We segment each slab into blocks/chunks.
    - For any given arena size, i have the number of slabs required. but how am i going to split them?
    - for now, lets assume the size of the arena is 1024 * 1024. that gives us 256 slabs of 4096 bytes each. (isnt that a bit too much). that means for each block of a certain size, i can have (256/8) = 32 slabs
    - number of blocks for each size is ( 32 * 4096 / sizeof(mem) )
    - The number of blocks for each size is: {0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100, 0x80} 
    - Malloc:
        1. User requests size.
        2. Iterate through the list of arrays and find the size closest to the requested one.
        3. Check for that bin's freelist. If its empty, check slabs's free count (free list is different than free count)
        4. Lazy slab allocation- Allocate appropriate number of slabs per bin and then only create more if `slab->free_count` becomes 0.

## GDB:
    - What do I really want to ask GDB?
    - Have my structs been initialised properly?
    - Are my calculations right?

## Visual Representation of my malloc:
    
    Arena --->  Bin1 (8B) --------> ----> Slab 1 ----> Block 1 -----> Block 2 ----> ... ----> Block 0x4000
                |                        |
                |                        ----> Slab 2 ----> Block 1 -----> Block 2 ----> ... ----> Block 0x4000
                |                            |
                |                            ----> Slab 3 ----> Block 1 -----> Block 2 ----> ... ----> Block 0x4000
                |                                .
                |                                .
                |                                .
                |                                ----> Slab 32 ----> Block 1 -----> Block 2 ----> ... ----> Block 0x4000
                |
                ---->  Bin2 (16B) ----> Block 1 -----> Block 2 ----> ... ----> Block 0x2000
                     |
                     .
                     .
                     .
                     |
                     ----> Bin3 (32B) ----> Block 1 -----> Block 2 ----> ... ----> Block 0x1000
                        |
                        .
                        .
                        .
                        |
                        ----> Bin8 (1024B) ----> Block 1 -----> Block 2 ----> ... ----> Block 0x80A
