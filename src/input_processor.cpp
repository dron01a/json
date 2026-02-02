#include "input_processor.h"

#include <cctype>

using namespace json::encodings;
using namespace json::io_base;
using namespace json::io::basic_processors;

std::string string_proc::operator()(i_input * src, i_decoder * decoder, size_t & line, size_t & col){
	return decoder->decode(src, line, col);
}

double digit_proc::operator()(i_input * _src, size_t & col){
	std::string result;
	char cur_char = _src->last_char();
	if (cur_char == '-') {
		result += cur_char;
		cur_char = _src->next_char();
		if (!std::isdigit(cur_char) && cur_char != '.') {
			//throw error(col, str, error_type::_invalid_number); // вставить ошибку
		}
	}
	while (std::isdigit(cur_char)) {
		result += cur_char;
		cur_char = _src->next_char();
	}
	if (cur_char == '.') {
		result += cur_char;
		cur_char = _src->next_char();
		while (std::isdigit(cur_char)) {
			result += cur_char;
			cur_char = _src->next_char();
		}
	}
	if (cur_char == 'e' || cur_char == 'E') {
		result += cur_char;
		cur_char = _src->next_char();
		if (cur_char == '+' || cur_char == '-') {
			result += cur_char;
			cur_char = _src->next_char();
		}
		while (std::isdigit(cur_char)) {
			result += cur_char;
			cur_char = _src->next_char();
		}
	}
	try {
		_src->step_back(1);
		col += result.size();
		return std::stod(result.c_str());
	}
	catch (std::exception & e) {
		// throw error(col, str, error_type::_invalid_number); // вставить ошибку 
	}
}

bool literal_proc::operator()(i_input * _src, size_t & col, std::string liter){
	char cur_char = _src->next_char();
	for (size_t i = 1; i < liter.size(); ++i) {
		if (cur_char != liter[i] || cur_char == '\0') {
			return false;
			// throw error(col, str, error_type::_literal_error); // вставить ошибку 
		}
		cur_char = _src->next_char();
	}
	col += liter.size();
	return true;
}
