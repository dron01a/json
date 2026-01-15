#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "json.h"

using namespace json;

int main() {
	json_parser p;
	json_value v = p.load_from_file("dest.json");
	json_writer w;
	w.write_to_stream(v, std::cout);
	w.write_to_file(v, "dest.json");
	return 0;
}