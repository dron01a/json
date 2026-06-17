#include <iostream>
#include <cassert>

#include "../tools/tests_framework.h"

#define _ALL_TESTS_RUN

// io_base tests
#include "../unit/input test.cpp"

//// encodings test
#include "../unit/ascii_decoder test.cpp"
#include "../unit/utf8_decoder test.cpp"

//// io tests
#include "../unit/base_input_processor test.cpp"

//// json value tests
#include "../unit/json_storage test.cpp"
#include "../unit/json_value test.cpp"

// dom_parser_test

// validator tests
#include "../unit/validator test.cpp"

int main() {
	tester::inst().run();
	return 0;
}
