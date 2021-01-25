#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "arena_pool.h"

using std::vector;
using std::unordered_map;
using std::cout;
using std::string;
using std::getline;
using std::istringstream;

using llutils::Rule;
using llutils::ArenaPool;

namespace llutils {

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
	unordered_map<string,Rule*> rule_map;
	vector<Rule> rules;
	ArenaPool rule_pool;
	vector<vector<string>> tests = { 
	{ "rule1",
		"rule2",
		"rule3 rule4",
		"\"abc\" \"def\" \"ghi\" rule5" },
	{ "rule2",
		"\"abc\" \"def\" \"ghi\" rule5",
		"\"abc\" \"def\" \"ghi\" rule5"
		},
	{ "observe//",
		"\"observe\"",
		"\"observes\"",
		"\"observing\"",
		"\"observed\""	},
	{ "detect//",
		"\"detect\"",
		"\"detects\"",
		"\"detecting\"",
		"\"detected\""	}};

	vector<vector<string>> grammar_tests = {
	{ "S",
		"sub vb obj",
		"sub va vb obj",
		"sub vb oc obj" },

	{ "sub",
		"\"dad\"",
		"\"mum\"",
		"\"town\"",
		"\"downstairs\"",
		"\"the\" \"key\"",
	},

	{ "va",
		"va-do",
		"va-have" },
	{ "va-do",
		"\"do\"",
		"\"does\"",
		"\"did\"",	},
	{ "va-have",
		"\"have\"",
		"\"has\"",
		"\"had\"" },

	{ "vb",
		"vb-give",
		"vb-take",
		"vb-go" },

	{ "vb-go",
		"\"go\"",
		"\"goes\"",
		"\"going\"",
		"\"went\""	},

	{ "vb-give",
		"\"give\"",
		"\"gives\"",
		"\"giving\"",
		"\"given\"",
		"\"gave\""	},

	{ "vb-take",
		"\"take\"",
		"\"takes\"",
		"\"taking\"",
		"\"took\"",
		"\"taken\""	},

	  { "vc",
		"\"to\"",
		"\"from\"", },

	};
	for (auto test : grammar_tests) {
		Rule* rule = llutils::make_rule(test);
		rules.push_back(*rule);
		rule_map[rule->name] = rule;
	}
	cout << "pass 1 : create the raw strings\n";
	for (Rule rule : rules) {
		uint32_t index = rule_pool.create_rule_index_pass1(rule);
		//cout << "got rule index " << index << "\n";
	}
	cout << "pass 2 : create the rules (with placeholders for references)\n";
	for (Rule rule : rules) {
		uint32_t index = rule_pool.create_rule_index_pass2(rule);
		//cout << "got rule index " << index << "\n";
	}
	cout << "pass 3 : replace the placeholders in the rules)\n";
	for (Rule rule : rules) {
		uint32_t index = rule_pool.create_rule_index_pass3(rule);
		//cout << "got rule index " << index << "\n";
	}
	uint32_t start_index = rule_pool.find_rule_index("S");
	cout << "got rule index for S : " << start_index << "\n";
	rule_pool.print_node(start_index, "");
	cout << "done\n";
}