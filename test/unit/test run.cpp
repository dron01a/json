#include <iostream>
#include <cassert>

#include "../tools/tests_framework.h"

#define _ALL_TESTS_RUN

#include "../unit/input test.cpp"
#include "../unit/json_storage test.cpp"
#include "../unit/json_value test.cpp"
#include "../unit/base_input_processor test.cpp"

int main() {
	tester::inst().run();
	return 0;
}
