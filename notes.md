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
