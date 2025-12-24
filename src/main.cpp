#include <iostream>
#include <string>
#include <vector>

#include "json.h"

using namespace json;

int main() {
	std::string json_string = "{  \"a\" : -123.65e+6 , \"obj\" : { \"b\" : { \"c\" : \"str\", \"d\" : false } } }";

	/*json_parser p;
	json_value v;
	json_value * arr;
	try {
		v = p.load_from_file(".\\test.json");
		arr = v.get("add");

	}
	catch (error_type & err) {
		std::cout << (int)err << std::endl;
	}
*/
	return 0;
}