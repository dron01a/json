#include "input_processor.h"

#include <cctype>

json::io::input_error::input_error(error_code code, size_t & line, size_t & col) : 
	base_error(error_category::input_error, line, col, form_message(code))
{}

std::string json::io::input_error::form_message(error_code code){
	return std::string();
}

json::io::token::token() : _type(token_type::_none) {}

json::io::token::token(const token & other) {
	copy_data(other);
}

json::io::token::token(token_type t) : _type(t) {}

json::io::token::token(double data) : _type(token_type::_number) {
	_data._double_data = data;
}

json::io::token::token(const char * data) : _type(token_type::_string) {
	_data._string_data = data;
}

json::io::token::token(const std::string & data) : _type(token_type::_string) {
	_data._string_data = data;
}

json::io::token::~token() {}

json::io::token & json::io::token::operator=(const token & other) {
	copy_data(other);
	return *this;
}

std::string json::io::token::string_data() const {
	if (_type == token_type::_string) {
		return _data._string_data;
	}
	return "";
}

double json::io::token::double_data() const {
	if (_type == token_type::_number) {
		return _data._double_data;
	}
	return 0.0;
}

json::io::token_type json::io::token::type() const {
	return _type;
}

void json::io::token::type(token_type t) {
	_type = t;
	switch (_type) {
	case token_type::_number:
		_data._double_data = 0;
		break;
	case token_type::_string:
		_data._string_data = "";
		break;
	}
}

void json::io::token::copy_data(const token & other) {
	_type = other._type;
	_data._double_data = 0;
	switch (other._type) {
	case token_type::_number:
		_data._double_data = other._data._double_data;
		break;
	case token_type::_string:
		_data._string_data = other._data._string_data;
		break;
	}
}

json::io::base_input_processor::base_input_processor(size_t & line, size_t & col) : _line(line), _col(col){}
	
json::io::token json::io::base_input_processor::parse_string(std::unique_ptr<encodings::i_decoder> & _decoder){
	std::string result_string;
	result_string.reserve(64);
	char32_t _c = _decoder->next_char();
	_col++;
	while (_c != '\"') {
		switch (_c){
		case std::char_traits<char>::eof():
			throw json::io::input_error(input_error::error_code::_invalid_string, _line, _col);
		case '\\':
			result_string += parse_escape(_decoder);
			break;
		default:
			result_string += _c;
			break;
		}
	}
	return token(result_string);
}

std::string json::io::base_input_processor::parse_escape(std::unique_ptr<encodings::i_decoder> & _decoder) {
	std::string result;
	char32_t _c = _decoder->next_char();
	_col++;
	switch (_c)	{
	case '"': result += '\"'; break;
	case '\\': result += '\\'; break;
	case '/': result += '/'; break;
	case 'b': result += '\b'; break;
	case 'f': result += '\f'; break;
	case 'n': result += '\n'; break;
	case 'r': result += '\r'; break;
	case 't': result += '\t'; break;
	case 'u':
		break;
	default:
		throw input_error(input_error::error_code::_invalid_escape, _line, _col);
	}
	return result;
}

std::string json::io::base_input_processor::parse_unicode(std::unique_ptr<encodings::i_decoder>& _decoder){
	std::string result;
	uint32_t code = parse_unicode_pair(_decoder);
	if (code >= 0xD800 && code <= 0xDBFF) { // проверяем на наличие суррогатной пары
		uint32_t low_code = parse_unicode_pair(_decoder);
		if (low_code < 0xDC00 || low_code > 0xDFFF) {
			throw input_error(input_error::error_code::_invalid_unicode_low_pair, _line, _col);
		}
		code = 0x10000 + ((code - 0xD800) << 10) + (low_code - 0xDC00); // cкрепляем пару
	}
	if (code <= 0x7F) {
		result += static_cast<char>(code);
	}
	else if (code <= 0x7F) {
		result += static_cast<char>(0xC0 | (code >> 6) & 0x1F);
		result += static_cast<char>(0x80 | code & 0x3F);
	}
	else if (code <= 0xFFFF) {
		result += static_cast<char>(0xE0 | (code >> 12) & 0x0F);
		result += static_cast<char>(0x80 | (code >> 6) & 0x3F);
		result += static_cast<char>(0x80 | code & 0x3F);
	}
	else if (code <= 0x10FFFF) {
		result += static_cast<char>(0xF0 | (code >> 18) & 0x07);
		result += static_cast<char>(0x80 | (code >> 12) & 0x3F);
		result += static_cast<char>(0x80 | (code >> 6) & 0x3F);
		result += static_cast<char>(0x80 | code & 0x3F);
	}
	return result;
}

uint32_t json::io::base_input_processor::parse_unicode_pair(std::unique_ptr<encodings::i_decoder>& _decoder){
	uint32_t result_code;
	for (size_t i = 0; i < 4; ++i) {
		char c = _decoder->next_char();
		_col++;
		if (c == std::char_traits<char>::eof()) {
			throw input_error(input_error::error_code::_invalid_unicode_char, _line, _col);
		}
		else if (c >= '0' && c <= '9') {
			result_code = (result_code << 4) | (c - '0');
		}
		else if (c >= 'a' && c <= 'f') {
			result_code = (result_code << 4) | (c - 'a' + 10);
		}
		else if (c >= 'A' && c <= 'F') {
			result_code = (result_code << 4) | (c - 'A' + 10);
		}
		else {
			throw input_error(input_error::error_code::_invalid_unicode_char, _line, _col);
		}
	}
	return result_code;
}

json::io::token json::io::base_input_processor::parse_number(std::unique_ptr<encodings::i_decoder>& _decoder){
	std::string result_digit;
	char cur_char = _decoder->current_char();
	if (cur_char == '-') {
		result_digit += cur_char;
		cur_char = _decoder->next_char();
		if (!std::isdigit(cur_char) && cur_char != '.') {
			input_error(input_error::error_code::_invalid_number, _line, _col);
		}
	}
	while (std::isdigit(cur_char)) {
		result_digit += cur_char;
		cur_char = _decoder->next_char();
	}
	if (cur_char == '.') {
		result_digit += cur_char;
		cur_char = _decoder->next_char();
		while (std::isdigit(cur_char)) {
			result_digit += cur_char;
			cur_char = _decoder->next_char();
		}
	}
	if (cur_char == 'e' || cur_char == 'E') {
		result_digit += cur_char;
		cur_char = _decoder->next_char();
		if (cur_char == '+' || cur_char == '-') {
			result_digit += cur_char;
			cur_char = _decoder->next_char();
		}
		while (std::isdigit(cur_char)) {
			result_digit += cur_char;
			cur_char = _decoder->next_char();
		}
	}
	try {
		_decoder->push_buff(static_cast<char32_t>(cur_char)); // возвращает не подошедший символ в буфер
		_col += result_digit.size();
		return token(std::stod(result_digit.c_str()));
	}
	catch (std::exception & e) {
		throw input_error(input_error::error_code::_invalid_number, _line, _col);
	}
}

bool json::io::base_input_processor::parse_literal(std::unique_ptr<encodings::i_decoder>& _decoder, const char * literal_str, size_t len){
	char cur_char = _decoder->next_char();
	for (size_t i = 1; i < len; ++i) {
		if (cur_char != literal_str[i] || cur_char == '\0') {
			throw input_error(input_error::error_code::_literal_error, _line, _col);
		}
		cur_char = _decoder->next_char();
	}
	_col += len;
	return true;
}

void json::io::base_input_processor::skip_space(std::unique_ptr<encodings::i_decoder>& _decoder){
	char cur_char = _decoder->next_char();
	while (cur_char == ' ' || cur_char == '\t' || cur_char == '\n' || cur_char == '\r') {
		switch (cur_char) {
		case ' ':
		case '\t':
			_col++;
			break;
		case '\n':
		case '\r':
			_col = 0;
			_line++;
			break;
		}
		cur_char = _decoder->next_char();
	}
}

void json::io::base_input_processor::skip_coments(std::unique_ptr<encodings::i_decoder>& _decoder){
	/*char c = _decoder->current_char();
	bool is_comment = false;
	bool multi_line_comment = false;
	if (c == '/') {
		c = _decoder->next_char();
		switch (c){
		case '*':
			multi_line_comment = true;
			is_comment = true;
			break;
		case '/':
			multi_line_comment = false;
			is_comment = true;
			break;
		default:
			is_comment = false;
			break;
		}
	}
	while (is_comment){
		if (multi_line_comment) {
			while (c != std::char_traits<char>::eof()) {
				if (c == '*') {
					c = _decoder->peek_char();
					if (c == '/') {
						multi_line_comment = false;
						_decoder->clear_peek_buff();
					}
				}
				c = _decoder->next_char();
			}
		}
		else {

		}

	}*/
}
