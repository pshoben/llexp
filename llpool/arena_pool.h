#ifndef ARENA_POOL_H
#define ARENA_POOL_H

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include "arena_allocator.h"

using std::string;
using std::vector;
using std::cout;

namespace llutils {

	class RuleEntry {
	public:
		RuleEntry(bool p_is_string, string p_val) : is_string(p_is_string), val(p_val) { /*print();*/ }
		bool is_string; // true if val is a leaf string
		string val; // q-name/VB
		int pos; // earley position
		void print() {
			cout << (this->is_string ? "\"" : "") << this->val << (this->is_string ? "\"" : "") << " ";
		}
	};

	class Rule {
	public:
		string name; // q-name/VB
		string tag;  // xyz
		string answer; // christopher-columbus
		vector<vector<RuleEntry>> rule_lines;
		void print() {
			cout << "\n\n" << this->name << ":\n";
			for (auto rule_line : this->rule_lines) {
				cout << "  ";
				for (auto token : rule_line) {
					cout << (token.is_string?"\"":"") << token.val << (token.is_string ? "\"" : "") << " ";
				}
				cout << "\n";
			}
		}
	};

	// place for storing packed rules and strings
	class ArenaPool {
	public:
		ArenaAllocator<10 * 1024> allocator;
		std::unordered_map<size_t, uint32_t> string_to_index; // map listing index of a string, used during population/conversion phases
		std::unordered_map<size_t, uint32_t> rule_name_to_index; // map listing index of a rule, used during population/conversion phases

		// for debug only:
		std::unordered_map<string, size_t> string_to_hash;

		// display the value at the index - if full is true print full rule otherwise print name only
		string print_at(uint32_t index, bool full);

		void print_node(uint32_t index, string indent);

		uint32_t* dup_word(uint32_t val);
		uint32_t dup_word_index(uint32_t val);

		// len must include null terminator (therefore min is 1)
		uint32_t* dup_cstring(const char* str, size_t len);

		// len must include null terminator (therefore min is 1)
		uint32_t dup_cstring_index(const char* str, size_t len);

		// len must include null terminator (therefore min is 1)
		// if the string exists in the pool, return its index
		// else add the string to the top of the pool and return its index
		uint32_t get_or_create_string_index(const char* cstr, size_t len);

		// if the rule already exists return an error - duplicate rule definitions are not permitted
		// else add the rule to the top of the pool and return its index
		uint32_t create_rule_index(const char* rule_name, size_t len);

		// create an entry for the rule
		// 1st pass : create pool entries only for the rule line entries that are strings
		// 2nd pass : create pool entries for the rule, but leave entries for links to children as placeholders.
		//            this pass assigns the correct amount of space in the pool for each rule, and assigns the index for the rule.
		// 3rd pass : populate each of the placeholders with the correct index of the child rule (now that it is known after 2nd pass)
		uint32_t create_rule_index_pass1(Rule & rule);
		uint32_t create_rule_index_pass2(Rule & rule);
		uint32_t create_rule_index_pass3(Rule & rule);

		uint32_t find_rule_index(const string rule_name);

		void traverse(uint32_t start_rule_index);

	};
}
#endif
