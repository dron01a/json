#include <iostream>
#include <string>
#include <vector>

#include "json.h"

using namespace json;

int main() {

	json_value jsv(false);
	jsv.as_num(10.12);
	jsv.as_array({ 2.123, 5.123123 });
	jsv.as_num(10.2);
	jsv.as_object({ { "a", 12.2 }, { "b", 1.2 } });
	return 0;
}