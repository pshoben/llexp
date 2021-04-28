#ifndef SEGMENT_ALLOCATOR_H
#define SEGMENT_ALLOCATOR_H
#include <cstdint>
#include <cstring>

#include <vector>
#include "arena_allocator.h"

using std::vector;

namespace llutils {
// properties
// nolock : usable only within a single thread
// nodelete : arena allocation means deletes happen all at once
// permalinks : POD objects remain at the same address after allocation
//               ( as a consequence, the size of the arena is fixed at creation time)
// non-contiguous : indexes reference across segments

template<uint32_t SIZE = 10 * 1024 * 1024>
class SegmentAllocator {
public:
	vector<llutils::ArenaAllocator<SIZE> *> arenas;
	uint16_t next_free_segment_index; // segment index to allocate at
	ArenaAllocator<SIZE>* top_segment=nullptr;

	SegmentAllocator()
	{
		top_segment = new ArenaAllocator<SIZE>();
		arenas.push_back(top_segment)
		next_free_segment_index=1;
	}

	// use standard dtor & copy/move ctors and assignment operators

	/** @return the arena_id + index of the start of the chunk
	*   @return 0 on out of space - (first word is reserved for this)
	*   used for 32bit internal references within the area */
	uint32_t allocate_index(uint32_t num_words_to_allocate)
	{
		uint16_t segment_index = top_segment.allocate_index(num_words_to_allocate);
		if (!segment_index) [[unlikely]] {
			// failed to allocate, current segment is full, create a new one
			top_segment = new ArenaAllocator<SIZE>();
			arenas.push_back(top_segment)
			next_free_segment_index++;
			segment_index = top_segment.allocate_index(num_words_to_allocate);
		}
		return (next_free_segment_index * (1 << 16)) + segment_index;
	}
	/** @return pointer to the start of the chunk
	*   @return nullptr on out of space */
	uint32_t * allocate(unsigned int num_words_to_allocate) {
		uint32_t * addr = top_segment.allocate(num_words_to_allocate);
		if (!addr) [[unlikely]] {
			// failed to allocate, current segment is full, create a new one
			top_segment = new ArenaAllocator<SIZE>();
			arenas.push_back(top_segment)
			next_free_segment_index++;
			addr = top_segment.allocate(num_words_to_allocate);
		}
		return addr;
	}
	/** if top_segment has not moved on from
	* prev_free_index then decrement top_segment by num_words_to_release
	* note : calling function must calculate prev_free_index
	* from the value passed as num_words_to_allocate to its most recent successful call to allocate,
	* plus the index returned from that call.
	*/
	void release_index(uint32_t prev_free_index, uint32_t num_words_to_release) {
		top_segment.release_index(prev_free_index, num_words_to_release);
		// TODO handle release after new segment
	}
	/** if address pointed to by top_segment has not moved on from
	* prev_free_index_address then decrement top_segment by num_words_to_release
	* note : calling function must calculate prev_free_index
	* from the value passed as num_words_to_allocate to its most recent successful call to allocate,
	* plus the index returned from that call.
	*/
	void release(uint32_t * prev_free_index_address, uint32_t num_words_to_release) {
		top_segment.release(prev_free_index_address, num_words_to_release);
	}
};
}
#endif