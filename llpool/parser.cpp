
#include "parser.h"

using std::vector;
using std::unordered_map;
using std::cout;
using std::string;
using std::getline;
using std::istringstream;

namespace parser {

	void Pool::add(vector<string> rule_lines) {
		int count = 0;
		string name = rule_lines[0];
		for (auto rule_line : rule_lines) {
			if (count > 0) {
				std::cout << "    " << rule_line << "\n";
			}
			count++;
		}
	}
}
using parser::Parser;

int main(void)
{
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
	Parser parser;
	for (auto rule : grammar_tests) {
		parser.pool.add(rule);
		int count = 0;
		for (auto rule_line : rule) {
			if (count == 0 ) {
				std::cout << rule_line << ":\n"; // first line is rule name
			} else {
				std::cout << "    " << rule_line << "\n";
			}
			count++;
		}
	}
	
}