#include <iostream>
#include <cassert>

#include "input_processor.h"
#include "json.h"

#include "../test/tools/tests_framework.h"
#include "../test//tools/mock_tools.h"

using namespace json;
using namespace json::io;
using namespace json::impl;

int main() {
	//tester::inst().run();
	
	dom_parser p;
	writer w;

	write_config wc;
	wc.sinax() = write_config::sinax_mode::XML;
	wc.set_flag(write_flags::format);

	parse_config pc;

	pc.error_halding() = parse_config::error_mode::collect;
	pc.encoding() = encoding::utf8;
	auto res = p.from_file("test.json", pc);

	w.to_stream(res.json_val, std::cout, wc);

	for (auto it = res.errors.begin(); it != res.errors.end(); it++) {
		std::cout << it->get()->what() << std::endl;
	}

	return 0;
}

