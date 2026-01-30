#include "encoding.h"


#include <iostream>

int main() {
	setlocale(LC_ALL, "Ru-ru");
	json::io_base::string_input * str_in = new json::io_base::string_input("\"ÆÎÏÀstring\\n\"");
	size_t i = 0, j = 0;
	json::encodings::utf8::decoder decod;
	json::encodings::ascii::encoder encod;
	str_in->next_char();
	std::string result = decod.decode(str_in, i, j);
	std::string result_out;
	json::io_base::string_output * str_out = new json::io_base::string_output(result_out);
	encod.encode(str_out, result);
	std::cout << result_out << std::endl;
	return 0;
}