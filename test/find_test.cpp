#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "json.h"

using namespace json;

int main() {
	std::string json_string = "{  \"a\" : -123.65e+6 , \"obj\" : { \"b\" : { \"c\" : \"str\", \"d\" : false } } }";
	std::vector<std::string> object_from_find_test{ "a", "b", "c", "d" };
	json_parser p;
	json_value jv = p.load_from_string(json_string);
	if (p.get_last_error().type != error_type::_none) {
		std::cout << "parse error!" << (int)p.get_last_error().type << std::endl;
	}
	for (auto obj : object_from_find_test) {
		json_value * res = jv.find(obj);
		assert(res != nullptr);
	}
	json_value * res = jv.find("sas");
	assert(res == nullptr);
	res = jv.find("bb");
	assert(res == nullptr);
	res = jv.find("s12");
	assert(res == nullptr);
	return 0;
}