#include <iostream>
#include <string>
#include <vector>

#include "json.h"

using namespace json;

int main() {
	std::string s = "{  \"a\" : -123.65e+6 , \"obj\" : { \"bb\" : { \"c\" : \"str\", \"d\" : false } } }";
	json_parser p;
	json_value v;
	try {
		v = p.load_from_file(".\\test.json");
	}
	catch (error_type & err) {
		std::cout << (int)err << std::endl;
	}
	return 0;
}