
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

	void ArenaPool::print_node(uint32_t index, string indent)
	{
		if (index >= allocator.next_free_index) {
			cout << indent << "out of bounds";
		}
		if (index < 4) {
			cout << indent << "too low - may not be magic number";
		}
		char* p = (char*)&(allocator.buffer[index++]);
		if (*p == 0x1) {
			// start of rule marker
			uint32_t rule_name_index = allocator.buffer[index++];
			char* p2 = (char*)&(allocator.buffer[rule_name_index]);
			cout << indent << "rule : " << p2 << "\n";
			p = (char*)&(allocator.buffer[index]);
			while (*p != 0x03) {
				// still inside outer loop (list of rule lines)
				cout << indent << "\n";
				while (*p != 0x02) {
					// still inside inner loop (list of rule line entries)
					uint32_t target_index = allocator.buffer[index];
					char* p2 = (char*)&(allocator.buffer[target_index]);
					if (*p2 == 0x1) {
						// this rule line entry is a rule name
						print_node(target_index, indent + "  ");
					}
					else {
						// this rule line entry is a string name
						cout << "\"" << p2 << "\" ";
					}
					p = (char*)&(allocator.buffer[++index]);
				}
				cout << "\n";
				p = (char*)&(allocator.buffer[++index]);
			}
			cout << "\n";
		
		} else {
			// this is a string, print it
			cout << indent << "\"" << string(p) << "\"\n";
		}
	}

	string ArenaPool::print_at(uint32_t index, bool full)
	{
		string ret;
		if (index >= allocator.next_free_index) {
			return "out of bounds";
		}
		if (index < 4) {
			return "too low - may not be magic number";
		}
		char* p = (char*)&(allocator.buffer[index]);
		if (*p == 0x1) {
			// start of rule marker
			uint32_t rule_name_index = allocator.buffer[index + 1];
			return "rule : " + print_at(rule_name_index, full);
		}
		if (*p < 0x4) {
			// magic number end of list etc.
			return "magic number";
		}
		//size_t len = strlen(p);
		// not a rule: treat as a string
		return string(p);
	}

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
			string_to_hash[str] = string_hash; // for debug
			// not found, add to top of pool
			uint32_t new_cstring_index = dup_cstring_index(cstr, len);
			if (!new_cstring_index) { return 0; /* failure */ }
			string_to_index[string_hash] = new_cstring_index;
			cout << "string not found \"" << cstr << "\" (" << string_hash << ") adding at index : " << new_cstring_index << "\n";
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
			string_to_hash[str] = string_hash; // for debug
			// not found, add to map and create the begin rule marker

			uint32_t new_rule_index = dup_word_index(0x1); // begin rule marker : magic string : 0x1
			if (!new_rule_index) { return 0; /* failure */}
			rule_name_to_index[string_hash] = new_rule_index;
			cout << "rule not found \"" << cstr << "\" (" << string_hash << ") adding at index : " << new_rule_index << "\n";
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
		// store the rule name in the string pool
		if (!get_or_create_string_index(rule.name.c_str(), rule.name.size() + 1)) {
			return 0; // failure
		}
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

		// store the index of the rule name string
		uint32_t rule_name_string_index = get_or_create_string_index(rule.name.c_str(), rule.name.size() + 1);
		if (!dup_word_index(rule_name_string_index))
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
		if (!dup_word_index(0x3)) // end outer list rule marker : magic string : 0x3
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
		cout << " rule \"" << rule.name << "\" found at start pos " << (pos-1) << " with marker " << allocator.buffer[pos-1] << "\n";

		uint32_t child_index = allocator.buffer[pos];
		cout << " at pos " << pos << " got rule name string ref : " << child_index << " " << print_at(child_index, false) << "\n";
		pos++;

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
					cout << " at pos " << pos << " : setting target string " << rule_line_entry.val << " = " << child_index << " " << print_at(child_index,false) << "\n";
					allocator.buffer[pos++] = child_index; // inject the reference in place
				}
				else {
					// it is a rule name, lookup the rule_name_to_index map
					auto it = rule_name_to_index.find(string_hash);
					if (it == rule_name_to_index.end()) {
						cout << " at pos " << pos << " : rule " << rule_line_entry.val << "(" << string_hash  << ") not found\n";
						allocator.buffer[pos++] = 0; // inject the unknown reference
						return 0; // failure - child rule name not found
					}
					uint32_t child_index = it->second;
					cout << " at pos " << pos << " : setting target rule " << rule_line_entry.val << " = " << child_index << " " << print_at(child_index, false) << "\n";

					allocator.buffer[pos++] = child_index; // inject the reference in place
				}
			}
			pos++; // skip the end of list marker
		}
		return 1; // success
	}

	uint32_t ArenaPool::find_rule_index(const string rule_name)
	{
		size_t string_hash = std::hash<string>{}(rule_name);
		auto it = rule_name_to_index.find(string_hash);
		if (it == rule_name_to_index.end()) {
			// not found - should already exist
			cout << " rule name not found \"" << rule_name << "\"\n";
			return 0; // failure
		}
		return it->second;
	}


};
