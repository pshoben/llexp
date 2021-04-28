#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <cstdint>
#include <variant>

using std::vector;
using std::string;
using std::cout;


namespace parser
{
	class RuleItem {
		bool is_leaf;
		string val; // if not leaf, is a rule name
	};
	class RuleLine {
		vector<RuleItem> items;
		int pos;
	};
	class Rule {
	public:
		string rule_name;
		vector<RuleLine> rule_lines;
	};
	class EarleyLine {
		vector<std::variant<string,EarleyItem *>> items;
		int pos;
	};
	class EarleyItem {
	public:
		bool is_leaf;
		string name;
		vector<EarleyLine> earley_lines;
	};

	class Pool {
	public:
		std::unordered_map<string, Rule> rule_map; // key is rule name
	};

	class Parser
	{
	public:
		Pool pool;
		vector<string> input_sentence;
		EarleyItem * top = make_earley_item("S");
		
		EarleyItem * make_earley_item(string rule_name) {
			EarleyItem* item = new EarleyItem();
			auto it = pool.rule_map.find(rule_name);
			if (it != pool.rule_map.end()) {
				Rule rule = it->second;
				item->is_leaf = false;
				item->name = rule.rule_name;


			}
			return item;
		}
	};
}
