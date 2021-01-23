#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H
#include <cstdint>
#include <cstring>


namespace llutils {
// properties
// nolock : usable only within a single thread
// nodelete : arena allocation means deletes happen all at once
// permalinks : POD objects remain at the same address after allocation
//               ( as a consequence, the size of the arena is fixed at creation time)
// contiguous : subsequent calls to allocate return contiguous addresses
// each call to allocate returns the next available word, and increments the top
// pointer by the number of bytes allocated
// allocations do not have to be all of the one size
// additionally - release call returns a number of bytes to the allocator.
// as long as no subsequent call to allocate has moved on the top pointer,
// this means that functions that do not know at allocation time exactly how much memory
// they need to allocate may grab the largest that
// might be needed, but then reduce the size of the request later, once the real size
// is known.

template<uint32_t SIZE = 10 * 1024 *1024>
class ArenaAllocator {
public:
	uint32_t buffer[SIZE];
	uint32_t next_free_index; // index to allocate at

	ArenaAllocator()
	:next_free_index(4) // first four words reserved
	{
		memset(buffer, 0, sizeof(buffer));
	}

	// use standard dtor & copy/move ctors and assignment operators

	/** @return the arena_id + index of the start of the chunk
	*   @return 0 on out of space - (first word is reserved for this)
	*   used for 32bit internal references within the area */
	uint32_t allocate_index(uint32_t num_words_to_allocate)
	{
		uint32_t prev_free_index = next_free_index;
		next_free_index += num_words_to_allocate;
		if (next_free_index >= SIZE) [[unlikely]]
			return 0; // out of space
		return prev_free_index;
	}
	/** @return pointer to the start of the chunk
	*   @return nullptr on out of space */
	uint32_t * allocate(unsigned int num_words_to_allocate) {
		uint32_t index = allocate_index(num_words_to_allocate);
		if (index == 0) [[unlikely]]
			return nullptr; // out of space
		return &(buffer[index]);
	}
	/** if next_free_index has not moved on from
	* prev_free_index then decrement next_free_index by num_words_to_release
	* note : calling function must calculate prev_free_index
	* from the value passed as num_words_to_allocate to its most recent successful call to allocate,
	* plus the index returned from that call.
	*/
	void release_index(uint32_t prev_free_index, uint32_t num_words_to_release) {
		// make sure free index has not moved on
		if (next_free_index == prev_free_index) {
			next_free_index -= num_words_to_release;
			memset(next_free_index, 0, num_words_to_release * 4);
		}
	}
	/** if address pointed to by next_free_index has not moved on from
	* prev_free_index_address then decrement next_free_index by num_words_to_release
	* note : calling function must calculate prev_free_index
	* from the value passed as num_words_to_allocate to its most recent successful call to allocate,
	* plus the index returned from that call.
	*/
	void release(uint32_t * prev_free_index_address, uint32_t num_words_to_release) {
		// make sure free index has not moved on
		if (prev_free_index_address == &(buffer[next_free_index])) {
			next_free_index -= num_words_to_release;
			memset(next_free_index, 0, num_words_to_release * 4);
		}
	}
};
}
#endif