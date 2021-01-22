#ifndef LLLIB_H
#define LLLIB_H

#include <cstdint>

using IndexType = unsigned int;

namespace llutils {

	enum ListStateEnum {
		CloseInner = 0x00,			// ],[  as in ..."wide"],[ ... of [["hello","wide"],["world"]] 
		CloseOuter = 0x01,			// ]]	as in ..."world"]] ... of [["hello","wide"],["world"]] 
		ContinueInner = 0x02		// ,    as in "hello",     ... of [["hello","wide"],["world"]] 
	};
	struct IndexHeaderType { // index type uses 3 control bits of 32 leaving 2^29 bits for the raw index value
		ListStateEnum list_state : 2;
		bool is_string : 1; // if unset, this value is IndexType , set it is SizedString
	};
	struct StringHeaderType {
		uint8_t len;
	};
	struct IndexType {
		union {
			IndexHeaderType header; // is_index_bit = 1
			unsigned int index;
		};
	};
};

#endif