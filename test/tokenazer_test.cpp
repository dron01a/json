#include <iostream>
#include <string>
#include <vector>

#include "json.h"

using namespace json;

int main() {

	std::vector<std::string> test_strings{
		"{ \"a\" : -123.65e+6 } , { \"bb\" : { \"c\" : \"str\", \"d\" : [1, 2] } }",
		"{ \"asd\" : 466fe+-r68 }",
		"{ \"array\" : [ 1, 56, 122, true, false, null ]}",
		"{ a : [ { \"key\" : 456 }, { \"key\" : 456 }, { \"key\" : 456 } ] }",
	};

	for (size_t i = 0; i < test_strings.size(); ++i) {
		tokenizer tt(new string_reader(test_strings[i]));
		try {
			tt.get_next_token();
			while (tt.get_last_token()._type != token_type::_end) {
				tt.get_next_token();
			}
		}
		catch (error_type & error) {
			std::cout << i << ") " << (int)error << std::endl;
		}
	}
	return 0;
}