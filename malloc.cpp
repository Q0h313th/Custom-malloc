#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <array>

/*
 * This is the design of a first-fit memory-allocator
 */

class Arena {
	private:
		// Bad portability to other systems but I dont care
		static const size_t PAGE_SIZE = 4096; 
		char *arena_pointer;
		size_t arena_size;
		size_t offset;
		const size_t MAX_NUM_SLABS;

		// Each free list contains a linked list of freed blocks. This is bin specific.
		struct Free_list {
			Free_list *next;
		};

		// Each slab contains a linked list of blocks.
		struct Slab {
			Slab *next;
			void *mem;
			size_t block_size;
			size_t block_count;
			int free_count;
		};

		/*
		 * Each bin contains a linked list of Slabs
		 * Each bin contains also the free list 
		 */
		
		struct Bin {
			Slab *slab_list;
			Free_list *free_list;
		};

		
		/* List of bins.
		 * the maximum size of an allocation is 512
		 * 0 can be allocated but is the minimum size for memory allocation
		 */
#define NUM_OF_BINS 8
		
		std::array<size_t, NUM_OF_BINS> bins_list { 0, 8, 16, 32, 64, 128, 256, 512 };

		/*
		 * Array of bin structs
		 */
		std::array<Bin, NUM_OF_BINS> bins;

	public:
		// Use long long for the size to prevent integer overflows
		explicit Arena(long long size = 1024 * 1024)
			: arena_pointer(nullptr), offset(0),
			arena_size( ( ( static_cast<size_t>(size) + PAGE_SIZE - 1 ) / PAGE_SIZE ) * PAGE_SIZE ),
			MAX_NUM_SLABS(arena_size / PAGE_SIZE)
		{
			
			if (size < 0){ throw std::invalid_argument("SHENANIGANS!!!"); } 
			
			// We can finally allocate the Arena
			write(1, "About to mmap", 13);
			arena_pointer = static_cast<char *>(mmap(nullptr, arena_size, PROT_READ | PROT_WRITE,
									MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
			std::cout << arena_pointer << std::endl;

			if (arena_pointer == MAP_FAILED){ perror("mmap"); exit(EXIT_FAILURE); }

			/* Initialize the Bins struct properly */
			for (size_t i = 0; i < NUM_OF_BINS; i++){
				bins[i].slab_list = nullptr;
				bins[i].free_list = nullptr;
			}
		}

		~Arena()
		{
			if (arena_pointer){ munmap(arena_pointer, arena_size); }
			
		}
		/* Round up "size" to the nearest multiple of PAGE_SIZE	
		 * This is moved to the initialiser list because you cant declare variables outside a function or something
		 * arena_size = ((static_cast<size_t>(size) + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
		 * max_num_slabs = arena_size / PAGE_SIZE;
		*/
};

int main(){
	try {
		Arena default_arena;
		std::cout << "Default arena created" << std::endl;

		Arena custom_arena(2 * 1024 * 1024);
		std::cout << "Custom arena created" << std::endl ;

		Arena bad_arena(-500);
		std::cout << "Bad arena!" << std::endl;

	}
	catch (const std::invalid_argument &e){
		std::cerr << "Invalid argument: " << e.what() << '\n';
	}
	catch (const std::exception &e){
		std::cerr << "Exception: " << e.what() << '\n';
	}

	return 0;

}


