#include <iostream>
#include <cassert>
#include "json_value.h"

#include "../tools/tests_framework.h"

using namespace json;
using namespace json::core::impl;

#define _ALL_TESTS_RUN

#include "../unit/json_storage test.cpp"
#include "../unit/json_value test.cpp"
#include "../unit/base_input_processor test.cpp"

int main() {
	tester::inst().run();
	return 0;
}
