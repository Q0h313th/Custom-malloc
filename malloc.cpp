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
		size_t offset;
		size_t arena_size;
		const size_t MAX_NUM_SLABS;
		const size_t NUM_SLABS_PER_BLOCK;

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

		
		/* array of bin sizes.
		 * the maximum size of an allocation is 512
		 * 0 can be allocated but is the minimum size for memory allocation
		 */
#define NUM_BINS 8
		
		std::array<size_t, NUM_BINS> bins_list { 8, 16, 32, 64, 128, 256, 512, 1024 };

		/*
		 * Array of bin structs
		 */
		std::array<Bin, NUM_BINS> bins;

		/*
		 * Array of block_counts corresponding to size in bins_list
		 */
		std::array<size_t, NUM_BINS> blocks_to_bins_list {};
		std::array<size_t, NUM_BINS> blocks_from_bins_list {0};

		int find_bin_index(size_t chunk_size){
			for (size_t i = 0; i < NUM_BINS; i++){
				if (chunk_size <= bins_list[i]){ return static_cast<int>(i); }
			}
			return -1;
		}
		Slab* create_new_slab(size_t index){
			size_t data_size = bins_list[index];

			void* slab_mem = arena_pointer + offset;
			offset += PAGE_SIZE;

			std::cout << "Created a slab object" << std::endl;

			Slab* slab = static_cast<Slab*>(slab_mem);
			slab->next = nullptr;
			slab->mem = static_cast<Slab*>(slab_mem) + sizeof(slab);
			slab->block_size = bins_list[index];
			slab->block_count = blocks_from_bins_list[index];
			slab->free_count = blocks_from_bins_list[index];
		}

		/*
		 *
		 *
		 *
		 *
		 *
		 *
		 *
		 */

	public:
		/* Use long long for the size to prevent integer overflows */
		explicit Arena(long long size = 1024 * 1024)
			: arena_pointer(nullptr), offset(0),
			arena_size( ( ( static_cast<size_t>(size) + PAGE_SIZE - 1 ) / PAGE_SIZE ) * PAGE_SIZE ),
			MAX_NUM_SLABS(arena_size / PAGE_SIZE),
			NUM_SLABS_PER_BLOCK(256 / NUM_BINS)
			//blocks_to_bins_list(blocks_per_size(bins_list))

		{
			
			if (size <= 0){ throw std::invalid_argument("SHENANIGANS!!!"); } 
			
			/* We can finally allocate the Arena */
			write(1, "About to mmap\n", 13);
			arena_pointer = static_cast<char *>(mmap(nullptr, arena_size, PROT_READ | PROT_WRITE,
									MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
			std::cout << arena_pointer << std::endl;

			if (arena_pointer == MAP_FAILED){ perror("mmap"); exit(EXIT_FAILURE); }

			/* Initialize the Bins struct properly */
			for (size_t i = 0; i < NUM_BINS; i++){
				bins[i].slab_list = nullptr;
				bins[i].free_list = nullptr;

				create_new_slab[i];
			}

			/*
			 * Calculate the number of blocks based on the bins_list array.
			 * The entire calculation is this:
			 * Total number of pages available in the arena is, for e.g.,: 1024 * 1024 / 4096 = 256
			 * 256 / 8 = 32 (32 pages available for each size)
			 * 32 * 4096 = total number of bytes
			 * 131072 / 8, 16, 32.... = number of blocks allocated to each size
			 */
			for (size_t i = 0; i < NUM_BINS; i++){
				blocks_to_bins_list[i] = ( ( ( size / PAGE_SIZE) / NUM_BINS ) * PAGE_SIZE ) / bins_list[i];
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
		
		void* allocate(size_t mem_size){
			size_t chunk_size;
			int chunk_index;
			if (mem_size == 0){ return nullptr; }
			/* 
			 * Calculate the size of the chunk from the size of the memory requested
			 */
			chunk_size = mem_size + sizeof(metadata);
			chunk_index = find_bin_index(chunk_size);
			if (chunk_index < 0){ return nullptr; }
			size_t chunk_index = static_cast<size_t>(chunk_index);

			// get free block, or create a new slab if it doesnt exist
			free_block = bins[chunk_index].free_list;
			if (!free_block){
				create_new_slab(chunk_index);
				free_block = bins[chunk_index].free_list;
			}
			
			// make the free_block->next the new head of the linked list
			bins[chunk_index].free_list = free_block->next;
		}
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


