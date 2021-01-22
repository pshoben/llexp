#ifndef LLPOOL_H
#define LLPOOL_H

#include <cstdint>
#include <string>

#include "lllib.h"

namespace llstr {

	class Pool {
		unsigned int slab[1000]; // TODO set size in ctor // all entries start on word boundaries
		unsigned int next_index = 1;
	public:
		// return the string value at the index
		template<int MaxLen>
		SizedStringType& get(IndexType index, int MaxLen) {
			return (String)
		}
		// allocate string at end of pool and return index (or 0 on error)
		IndexType appendString(std::string from) {
			if (next_index >= 1000) {
				// error - pool is full 
				return 0;
			}
			// allocate space for the string at the end of the slab, and move next_index to next free word
			size_t len = strlen(from.c_str());
			if (len > 31)
				len = 31;
			size_t rounded_len = len >> 2 + 1; // round up to nearest 4 bytes
			size_t len_words = rounded_len >> 2;
			switch (rounded_len) {
			case 4: {
				SizedStringType<4>& str = (SizedStringType<4>&)slab[next_index];
				str.init(from, len);
				break;
			}
			case 8: {
				SizedStringType<8>& str = (SizedStringType<8>&)slab[next_index];
				str.init(from, len);
				break;
			}
			case 12: {
				SizedStringType<12>& str = (SizedStringType<12>&)slab[next_index];
				str.init(from, len);
				break;
			}
			case 16: {
				SizedStringType<16>& str = (SizedStringType<16>&)slab[next_index];
				str.init(from, len);
				break;
			}
			case 20: {
				SizedStringType<20>& str = (SizedStringType<20>&)slab[next_index];
				str.init(from, len);
				break;
			}
			case 24: {
				SizedStringType<24>& str = (SizedStringType<24>&)slab[next_index];
				str.init(from, len);
				break;
			}
			case 28: {
				SizedStringType<28>& str = (SizedStringType<28>&)slab[next_index];
				str.init(from, len);
				break;
			}
			case 32: {
				SizedStringType<32>& str = (SizedStringType<32>&)slab[next_index];
				str.init(from, len);
				break;
			}
			default:
				// invalid string length
				return 0;
			}
			next_index += len_words;
		}
	}

#endif