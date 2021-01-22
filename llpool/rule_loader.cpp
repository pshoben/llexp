#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "arena_pool.h"

using std::vector;
using std::cout;
using std::string;
using std::getline;
using std::istringstream;

namespace llutils {

	vector<Rule> rules;
	ArenaPool rule_pool;

	Rule * make_rule(vector<string> rule_lines)
	{
		// remaining lines are list of RuleEntries
		Rule * rule = new Rule();
		rule->name = rule_lines[0];

		bool first_line = true;
		for (auto it = rule_lines.begin(); it != rule_lines.end(); ++it) {
			// ignore the first line - it is the rule name
			if (!first_line) {
				string token;
				istringstream iss(*it);
				vector<RuleEntry> rule_entries{};
				while (std::getline(iss, token, ' ')) {
					if (token.c_str()[0] == '\"') {
						// this is a raw string token - tag it and remove the quotes
						RuleEntry string_entry{ true, token.substr(1, token.size() - 2) };
						rule_entries.push_back(string_entry);
					} else {
						RuleEntry rule_entry{ false, token };
						rule_entries.push_back(rule_entry);
					}
				}
				rule->rule_lines.push_back(rule_entries);
			}
			first_line = false;
		}
		rule->print();
		return rule;
	}

	//uint32_t load_rules()
	//{
	//	// 1st pass : add raw strings into the pool
	//	for (auto rule : rules) {
	//		if (!rule_pool.create_rule_index_pass1(rule)) {
	//			return 0; // failure
	//		}
	//	}
	//	// 2nd pass : add rules into the pool
	//	for (auto rule : rules) {
	//		if (!rule_pool.create_rule_index_pass2(rule)) {
	//			return 0; // failure
	//		}
	//	}
	//	// 3rd pass : add connections/references between the rules
	//	for (auto rule : rules) {
	//		if (!rule_pool.create_rule_index_pass3(rule)) {
	//			return 0; // failure
	//		}
	//	}
	//	return 1;
	//}
}

int main(void)
{
	vector<string> test =     { "rule1",
								"rule2",
								"rule3 rule4",
								"\"abc\" \"def\" \"ghi\" rule5" };
	llutils::Rule * rule = llutils::make_rule(test);
	cout << "hello world\n";
}