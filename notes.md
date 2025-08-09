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
