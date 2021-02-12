
//#include <unordered_map>
#include <iostream>
//#include <string>
#include <vector>
#include <chrono>

#include "arena_pool.h"

//using std::unordered_map;
//using std::string;
//using std::vector;
//using std::cout;

using namespace std;

using namespace llutils;

ArenaPool pool;
const char* my_strings[] = { "abc", "defg", "", "supercalifragilisticexpialodocious" };
size_t my_stringlens[] = { sizeof("abc"), sizeof("def"),sizeof(""),sizeof("supercalifragilisticexpialodocious" )};
vector<char*> myresults;

void fill_pool() {
	// keep adding strings until arena is full
	while (true) {
		int count = 0;
		for (auto pc : my_strings) {
			char* result_address = (char*)pool.dup_cstring(pc, my_stringlens[count++] + 1);
			if (result_address) {
				myresults.push_back(result_address);
			}
			else {
				return;
			}
		}
	}
}

void fill_pool2() {
	// keep adding strings until arena is full or time up
	int count2 = 0;
	while (count2 < 10) {
		int count = 0;
		for (auto pc : my_strings) {
			char* result_address = (char*)(uint64_t)pool.get_or_create_string_index(pc, my_stringlens[count++] + 1);
			if (result_address) {
				myresults.push_back(result_address);
			}
			else {
				return;
			}
		}
		count2++;
	}
}


int main(void) {

	auto t1 = std::chrono::high_resolution_clock::now();
	fill_pool2();
	auto t2 = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << " function duration in micros: " << duration << "\n";
	
	//for (auto x : myresults) {
	//	cout << x << "\n";
	//}
	cout << "done" << "\n";
}
