#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "json.h"

using namespace json;

int main() {
	//json_parser p;
	//json_value v = p.load_from_file("test.json");
	//json_writer w(format());
	////w.config().set_flag(write_flags::trailing_commas);
	//w.write_to_stream(v, std::cout);
	//w.write_to_file(v, "dest.json");

	json_doc doc; 
	doc.load("test.json", true);
	json_value * val = doc.root();

	/*for (size_t i = 0; i < 15000; ++i ) {
		val->add(std::to_string(i), json_value((double)i));
	
	}*/

	jv_pointer p = val->find("n");

	json_pointer_array res = val->select("string");

	//doc.save("test.json");

	return 0;
}

