
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <string>
#include <unordered_map>

#include "arena_pool.h"

using std::unordered_map;
using std::string;
using std::cout;

namespace llutils {

	// len must include null terminator (therefore min is 1)
	uint32_t* ArenaPool::dup_word(uint32_t val)
	{
		uint32_t* new_word = allocator.allocate(1);
		if (new_word) [[likely]] {
			*new_word = val;
		}
		return new_word;
	}
	uint32_t ArenaPool::dup_word_index(uint32_t val)
	{
		uint32_t* new_word = dup_word(val);
		if (new_word) [[likely]] {
			// convert address back to index
			return (uint32_t)((uint32_t*)new_word - (uint32_t*)&(allocator.buffer));
		} else {
			return 0;
		}
	}
	// len must include null terminator (therefore min is 1)
	uint32_t* ArenaPool::dup_cstring(const char* str, size_t len)
	{
		uint32_t padded_size_words = ((--len) >> 2) + 1; // size in words 
		uint32_t* new_cstring_addr = allocator.allocate(padded_size_words);
		if (new_cstring_addr) [[likely]] {
			memcpy(new_cstring_addr, str, len);
		}
		return new_cstring_addr;
	}
	// len must include null terminator (therefore min is 1)
	uint32_t ArenaPool::dup_cstring_index(const char* str, size_t len)
	{
		uint32_t* new_cstring_addr = dup_cstring(str, len);
		// convert address back to index
		return (uint32_t)((uint32_t*)new_cstring_addr - (uint32_t*)&(allocator.buffer));
	}
	// len must include null terminator (therefore min is 1)
	// if the string exists in the pool, return its index
	// else add the string to the top of the pool and return its index
	uint32_t ArenaPool::get_or_create_string_index(const char* cstr, size_t len)
	{
		string str{ cstr };
		size_t string_hash = std::hash<string>{}(cstr);
		auto it = string_to_index.find(string_hash);
		if (it != string_to_index.end()) {
			// already exists
			cout << "found \"" << cstr << "\" at index : " << it->second << "\n";
			return it->second;
		}
		else {
			// not found, add to top of pool
			uint32_t new_cstring_index = dup_cstring_index(cstr, len);
string_to_index[string_hash] = new_cstring_index;
cout << "not found \"" << cstr << "\" adding at index : " << new_cstring_index << "\n";
return new_cstring_index;
		}
	}
	// len must include null terminator (therefore min is 1)
	// if the string exists in the pool, return its index
	// else add the string to the top of the pool and return its index
	uint32_t ArenaPool::create_rule_index(const char* cstr, size_t len)
	{
		string str{ cstr };
		size_t string_hash = std::hash<string>{}(cstr);
		auto it = rule_name_to_index.find(string_hash);
		if (it != rule_name_to_index.end()) {
			// already exists
			cout << "duplicate rule name found \"" << cstr << "\" at index : " << it->second << "\n";
			return 0; // failure
		}
		else {
			// not found, add to map and create the begin rule marker
			uint32_t new_rule_index = dup_word_index(0x1); // begin rule marker : magic string : 0x1
			if (!new_rule_index) {
				return 0; // failure
			}
			return new_rule_index;
		}
	}
	// create an entry for the rule
	// 1st pass : create pool entries only for the rule line entries that are strings
	// 2nd pass : create pool entries for the rule, but leave entries for links to children as placeholders.
	//            this pass assigns the correct amount of space in the pool for each rule, and assigns the index for the rule.
	// 3rd pass : populate each of the placeholders with the correct index of the child rule (now that it is known after 2nd pass)
	uint32_t ArenaPool::create_rule_index_pass1(Rule& rule)
	{
		// first pass: create any new strings in the pool (before the start of the rule proper)
		for (auto rule_line : rule.rule_lines) {
			for (auto rule_line_entry : rule_line) {
				if (rule_line_entry.is_string) {
					if (!get_or_create_string_index(rule_line_entry.val.c_str(), rule_line_entry.val.size() + 1)) {
						return 0; // failure
					}
				}
			}
		}
		return 1; // success
	}

	// pass2 creates the rule and returns its index
	uint32_t ArenaPool::create_rule_index_pass2(Rule& rule)
	{
		// create (if not exist) the string entries for the rule name etc.
		// not sure if these are strictly needed
		// uint32_t new_rule_name_string_index = get_or_create_string_index((const char*)rule.name.c_str(), rule.name.size() + 1);
		//uint32_t new_rule_tag_string_index = get_or_create_string_index((const char*)rule.tag.c_str(), rule.tag.size() + 1);
		//uint32_t new_rule_answer_string_index = get_or_create_string_index((const char*)rule.answer.c_str(), rule.answer.size() + 1);
		//if (!new_rule_tag_string_index || !new_rule_answer_string_index) {
		//	return 0; // failure
		//}

		// allocate the rule header info
		uint32_t new_rule_index = create_rule_index((const char*)rule.name.c_str(), rule.name.size() + 1);
		if (!new_rule_index)
			return 0; // failure

		//uint32_t new_rule_tag_index = dup_word_index(new_rule_tag_string_index);
		//uint32_t new_rule_answer_index = dup_word_index(new_rule_answer_string_index);
		//if (!new_rule_name_index || !new_rule_tag_index || !new_rule_answer_index) {
		//	return 0; // failure
		//}
		// second pass: all leaf string entries already exist, now populate the rule with their indexes
		for (auto rule_line : rule.rule_lines) {
			for (auto rule_line_entry : rule_line) {
				// create empty placeholder - will be populated in pass 3
				if (!dup_word_index(0x0))
					return 0; // failure
			}
			// end of list marker is 32-bit null terminator word - TODO fix this
			if (!dup_word_index(0x2)) // end list rule marker : magic string : 0x2
				return 0; // failure
		}
		// end of list marker is 32-bit null terminator word - TODO fix this
		if (!dup_word_index(0x2)) // end list rule marker : magic string : 0x2
			return 0; // failure
		return new_rule_index;
	}

	uint32_t ArenaPool::create_rule_index_pass3(Rule& rule)
	{
		size_t string_hash = std::hash<string>{}(rule.name);
		auto it = rule_name_to_index.find(string_hash);
		if (it == rule_name_to_index.end()) {
			// not found - should already exist
			cout << " rule name not found \"" << rule.name << "\"\n";
			return 0; // failure
		}
		uint32_t pos = it->second + 1; // skip the start rule marker
		// third pass: populate the child references in place with the found index
		for (auto rule_line : rule.rule_lines) {
			for (auto rule_line_entry : rule_line) {
				size_t string_hash = std::hash<string>{}(rule_line_entry.val);
				if (rule_line_entry.is_string) {
					// it is a rule name, lookup the string_to_index map
					auto it = string_to_index.find(string_hash);
					if (it == string_to_index.end()) {
						return 0; // failure - child string not found
					}
					uint32_t child_index = it->second;
					allocator.buffer[pos++] = child_index; // inject the reference in place
				}
				else {
					// it is a rule name, lookup the rule_name_to_index map
					auto it = rule_name_to_index.find(string_hash);
					if (it == string_to_index.end()) {
						return 0; // failure - child rule name not found
					}
					uint32_t child_index = it->second;
					allocator.buffer[pos++] = child_index; // inject the reference in place
				}
			}
			pos++; // skip the end of list marker
		}
		return 1; // success
	}
};
