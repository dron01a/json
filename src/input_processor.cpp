#include "input_processor.h"

#include <cctype>

json::io::input_error::input_error(error_code code, size_t & line, size_t & col) : 
	base_error(error_category::input_error, line, col, form_message(code))
{}

std::string json::io::input_error::form_message(error_code code){
	std::string result;
	switch (code){
	case json::io::input_error::error_code::_error_token:
		result += "error token";
		break;
	case json::io::input_error::error_code::_invalid_string:
		result += "invalid string";
		break;
	case json::io::input_error::error_code::_invalid_number:
		result += "invalid number";
		break;
	case json::io::input_error::error_code::_invalid_number_format:
		result += "invalid number format";
		break;
	case json::io::input_error::error_code::_invalid_escape:
		result += "escape processing error";
		break;
	case json::io::input_error::error_code::_invalid_unicode_char:
		result += "unicode char processing error";
		break;
	case json::io::input_error::error_code::_invalid_unicode_low_pair:
		result += "unicode low pair processing error";
		break;
	case json::io::input_error::error_code::_literal_error:
		result += "invalid literal";
		break;
	}
	return result;
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
	new (&_data._string_data) std::string(data);
}

json::io::token::token(const std::string & data) : _type(token_type::_string) {
	new (&_data._string_data) std::string(data);
}

json::io::token::~token() {
	if (_type == token_type::_string) {
		_data._string_data.~basic_string();
	}
}

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
		new (&_data._string_data) std::string("");
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
		new (&_data._string_data) std::string(other._data._string_data);
		break;
	}
}

json::io::base_input_processor::base_input_processor(size_t & line, size_t & col) : _line(line), _col(col){}

json::io::base_input_processor::base_input_processor(const base_input_processor & bip) 
	: _line(bip._line), _col(bip._col){}
	
json::io::token json::io::base_input_processor::parse_string(encodings::i_decoder_ptr_ref _decoder){
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
		_c = _decoder->next_char();
		_col++;
	}
	return token(result_string);
}

std::string json::io::base_input_processor::parse_escape(encodings::i_decoder_ptr_ref _decoder) {
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

std::string json::io::base_input_processor::parse_unicode(encodings::i_decoder_ptr_ref _decoder){
	std::string result;
	uint32_t code = parse_unicode_pair(_decoder);
	if (code >= 0xD800 && code <= 0xDBFF) { // проверяем на наличие сурогатной пары
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

uint32_t json::io::base_input_processor::parse_unicode_pair(encodings::i_decoder_ptr_ref _decoder){
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

json::io::token json::io::base_input_processor::parse_number(encodings::i_decoder_ptr_ref _decoder){
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

json::io::token json::io::base_input_processor::parse_literal(encodings::i_decoder_ptr_ref _decoder, const char * literal_str, token_type type){
	char cur_char = _decoder->current_char();
	while (*literal_str != '\0') {
		if (cur_char != *literal_str) {
			throw input_error(input_error::error_code::_literal_error, _line, _col);
		}
		cur_char = _decoder->next_char();
		*literal_str++;
	}
	return token(type);
}

void json::io::base_input_processor::skip_space(encodings::i_decoder_ptr_ref _decoder){
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

void json::io::base_input_processor::skip_coments(encodings::i_decoder_ptr_ref _decoder){
	char c = _decoder->current_char();
	while (c != std::char_traits<char>::eof()) {
		if (c == '/') {
			c = _decoder->next_char();
			switch (c){
			case '*':
				while (c != std::char_traits<char>::eof()) {
					skip_space(_decoder);
					c = _decoder->next_char();
					_col++;
					if (c == '*') {
						c = _decoder->next_char();
						_col++;
						if (c == '/') {
							break;
						}
					}
				}
				break;
			case '/':
				while (c != std::char_traits<char>::eof() && c != '\n' && c != '\r') {
					c = _decoder->current_char();
					_col++;
				}
				break;
			}
		}
		else {
			_decoder->push_buff(static_cast<char32_t>(c));
			break;
		}
		c = _decoder->next_char();
	}
}

json::io::json_input_processor::json_input_processor(size_t & line, size_t & col) 
	: base_input_processor(line, col){}

json::io::json_input_processor::json_input_processor(const json_input_processor & jip)
	: base_input_processor(jip._line, jip._col){}

json::io::token json::io::json_input_processor::next_token(encodings::i_decoder_ptr_ref _decoder){
	skip_space(_decoder);
	skip_coments(_decoder);
	char c = _decoder->next_char();
	_col++;
	switch (c){
	case std::char_traits<char>::eof(): return token(token_type::_end);
	case '{': return token(token_type::_open_curly_brt);
	case '}': return token(token_type::_close_curly_brt);
	case '[': return token(token_type::_open_square_brt);
	case ']': return token(token_type::_close_square_brt);
	case ':': return token(token_type::_colon);
	case ',': return token(token_type::_comma);
	case '\"': return parse_string(_decoder);
	case '-':
	case '0': case '1': case '2': case '3': case '4': 
	case '5': case '6': case '7': case '8': case '9':
		return parse_number(_decoder);
	case 't':
		/*if (parse_literal(_decoder, "true", 5)) {
			return token(token_type::_true);
		}
		throw input_error(input_error::error_code::_literal_error, _line, _col);*/
		return parse_literal(_decoder, "true", token_type::_true);
	case 'f':
		/*if (parse_literal(_decoder, "false", 6)) {
			return token(token_type::_false);
		}
		throw input_error(input_error::error_code::_literal_error, _line, _col);*/
		return parse_literal(_decoder, "false", token_type::_false);
	case 'n':
		/*if (parse_literal(_decoder, "null", 5)) {
			return token(token_type::_null);
		}
		throw input_error(input_error::error_code::_literal_error, _line, _col);*/
		return parse_literal(_decoder, "null", token_type::_null);
	default: throw input_error(input_error::error_code::_error_token, _line, _col);
	}
}
