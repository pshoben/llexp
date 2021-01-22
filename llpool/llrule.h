#ifndef LLRULE_H
#define LLRULE_H

#include <cstdint>
#include <string>
#include <vector>
#include "lllib.h"

using std::vector;

namespace llutils {

	struct Rule {
		IndexType name;
		IndexType tag;
		vector<vector<IndexType>> body;
	};

	/* size must have value 3 + 4x */ // TODO check if  packed
	/* template parameter OuterLen is the size (number of elements) of the outer array */
	template <typename T, uint8_t OuterLen>
	struct PackedVectorVector {
		uint8_t size; // storage requirements in Words
		uint8_t start[OuterLen];
		T body[];
	};

	/* create a packedVectorVector from a vector of vectors
	 * and return the number of words required by the packed structure */
	template <typename T, uint8_t OuterLen>
	uint32_t packVectorVector(PackedVectorVector<T,OuterLen> * dest, vector<vector<T>> & src) {
		dest->size = 0;
		uint32_t len = 0;
		for (auto outer_iter : src) {
			dest->start[dest->size] = len;
			for (auto inner_iter : outer_iter) {
				dest->body[len++] = *inner_iter;
			}
			dest->size++;
		}
		return len * sizeof(T) + Size + 1;
	}

	using PackedVectorVector_FuncPtrType = uint32_t(*)(PackedVectorVector<uint32_t, 2>*, vector<vector<uint32_t>>&) ;
	auto fptr = &packVectorVector<uint32_t, 1>;
	uint32_t(* fptr2)(PackedVectorVector<uint32_t, 2>* dest, vector<vector<uint32_t>>& src) = &packVectorVector<uint32_t, 2>;
	uint32_t(* fptr3)(PackedVectorVector<uint32_t, 3>* dest, vector<vector<uint32_t>>& src) = &packVectorVector<uint32_t, 3>;

	PackedVectorVector_FuncPtrType fptr4;

	PackedVectorVector_FuncPtrType fptr3s[] = { &packVectorVector<uint32_t,1>, &packVectorVector<uint32_t,4> };

	//				 &packVectorVector<uint32_t,2> };

	struct PackedRule {
		IndexType name;
		IndexType tag;
		PackedVectorVector<IndexType> body;
	};

	/* create a packed rule at dest, that is a copy of src*/
	void packRule(PackedRule& dest, Rule& src) {
		dest.name = src.name;
		dest.tag = src.tag;
		packVectorVector<IndexType>(dest.body, src.body)
	}


}


#endif