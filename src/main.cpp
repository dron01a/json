#include <iostream>
#include <string>
#include <vector>

#include "json.h"

using namespace json;

int main() {

	std::string s = "{ \"a\" , -123.65e+6 }";

	string_reader * r = new string_reader(s);
	tokenizer tt(r); 
	
	std::vector<token> vt;

	tt.get_nex_token();
	while (tt.get_last_token()._type != token_type::_end) {
		vt.push_back(tt.get_last_token());
		tt.get_nex_token();
	}

	return 0;
}