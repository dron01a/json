#include "core\input_processor.h"

#include <cctype>

using namespace json; 
using namespace json::io;
using namespace json::io::fsm;
using namespace json::io_base;
using namespace json::encodings;

input_error::input_error(error_code code, size_t & line, size_t & col) : 
	base_error(error_category::input_error, line, col, form_message(code))
{}

std::string input_error::form_message(error_code code){
	switch (code){
	case input_error::error_code::_invalid_token:
		return "error token";
	case input_error::error_code::_invalid_string:
		return "invalid string";
	case input_error::error_code::_invalid_number:
		return "invalid number";
	case input_error::error_code::_invalis_hex_number:
		return "invalid hex number";
	case input_error::error_code::_invalid_number_format:
		return "invalid number format";
	case input_error::error_code::_invalid_escape:
		return "escape processing error";
	case input_error::error_code::_invalid_unicode_char:
		return "unicode char processing error";
	case input_error::error_code::_invalid_unicode_low_pair:
		return "unicode low pair processing error";
	case input_error::error_code::_literal_error:
		return "invalid literal";
	}
}

token::token() : _type(token_type::_none) {}

token::token(const token & other) {
	copy_data(other);
}

token::token(token_type t) : _type(t) {}

token::token(double data) : _type(token_type::_number) {
	_data._double_data = data;
}

token::token(const char * data) : _type(token_type::_string) {
	new (&_data._string_data) std::string(data);
}

token::token(const std::string & data) : _type(token_type::_string) {
	new (&_data._string_data) std::string(data);
}

token::~token() {
	if (_type == token_type::_string) {
		_data._string_data.~basic_string();
	}
}

token & token::operator=(const token & other) {
	copy_data(other);
	return *this;
}

std::string token::string_data() const {
	if (_type == token_type::_string) {
		return _data._string_data;
	}
	return "";
}

void token::string_data(std::string data){
	if (_type != token_type::_string) {
		new (&_data._string_data) std::string(data);
		_type = token_type::_string;
	}
	else {
		_data._string_data = data;
	}
}

double token::double_data() const {
	if (_type == token_type::_number) {
		return _data._double_data;
	}
	return 0.0;
}

void token::double_data(double data){
	if (_type == token_type::_string) {
		_data._string_data.~basic_string();
	}
	_type = token_type::_number;
	_data._double_data = data;
}

token_type token::type() const {
	return _type;
}

void token::type(token_type t) {
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

void token::copy_data(const token & other) {
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

base_input_processor::base_input_processor() : _line(0), _col(0){}

base_input_processor::base_input_processor(const base_input_processor & bip) 
	: _line(bip._line), _col(bip._col){}

size_t base_input_processor::line(){
	return _line;
}

size_t base_input_processor::col(){
	return _col;
}
	
token base_input_processor::parse_string(i_decoder_ptr_ref _decoder, char quote_char){
	std::string result_string;
	result_string.reserve(64);
	char32_t _c = _decoder->next_char();
	_col++;
	while (_c != quote_char) {
		switch (_c){
		case std::char_traits<char>::eof():
			throw input_error(input_error::error_code::_invalid_string, _line, _col);
		case '\\':
			result_string += parse_escape(_decoder);
			break;
		default:
			//result_string += _c;
			if (_c <= 0x7F) {
				result_string += static_cast<char>(_c);
			}
			else if (_c <= 0x7FF) {
				result_string += static_cast<char>(0xC0 | (_c >> 6) & 0x1F);
				result_string += static_cast<char>(0x80 | _c & 0x3F);
			}
			else if (_c <= 0xFFFF) {
				result_string += static_cast<char>(0xE0 | (_c >> 12) & 0x0F);
				result_string += static_cast<char>(0x80 | (_c >> 6) & 0x3F);
				result_string += static_cast<char>(0x80 | _c & 0x3F);
			}
			else if (_c <= 0x10FFFF) {
				result_string += static_cast<char>(0xF0 | (_c >> 18) & 0x07);
				result_string += static_cast<char>(0x80 | (_c >> 12) & 0x3F);
				result_string += static_cast<char>(0x80 | (_c >> 6) & 0x3F);
				result_string += static_cast<char>(0x80 | _c & 0x3F);
			}
			break;
		}
		_c = _decoder->next_char();
		_col++;
	}
	return token(result_string);
}

std::string base_input_processor::parse_escape(i_decoder_ptr_ref _decoder) {
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
	case 'u': result += parse_unicode(_decoder);
		break;
	default:
		throw input_error(input_error::error_code::_invalid_escape, _line, _col);
	}
	return result;
}

std::string base_input_processor::parse_unicode(i_decoder_ptr_ref _decoder){
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
	else if (code <= 0x7FF) {
		result += static_cast<uint8_t>(0xC0 | (code >> 6) & 0x1F);
		result += static_cast<uint8_t>(0x80 | code & 0x3F);
	}
	else if (code <= 0xFFFF) {
		result += static_cast<uint8_t>(0xE0 | (code >> 12) & 0x0F);
		result += static_cast<uint8_t>(0x80 | (code >> 6) & 0x3F);
		result += static_cast<uint8_t>(0x80 | code & 0x3F);
	}
	else if (code <= 0x10FFFF) {
		result += static_cast<uint8_t>(0xF0 | (code >> 18) & 0x07);
		result += static_cast<uint8_t>(0x80 | (code >> 12) & 0x3F);
		result += static_cast<uint8_t>(0x80 | (code >> 6) & 0x3F);
		result += static_cast<uint8_t>(0x80 | code & 0x3F);
	}
	return result;
}

uint32_t base_input_processor::parse_unicode_pair(i_decoder_ptr_ref _decoder){
	uint32_t result_code = 0;
	for (size_t i = 0; i < 4; ++i) {
		uint8_t c = _decoder->next_char();
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

token base_input_processor::parse_number(i_decoder_ptr_ref _decoder){
	auto res = _dp_fsm.run(_decoder, _line, _col);
	if (res.second) {
		return token(res.first);
	}
	throw input_error(input_error::error_code::_invalid_number, _line, _col);
}

token base_input_processor::parse_literal(i_decoder_ptr_ref _decoder, const char * literal_str, token_type type){
	int cur_char = _decoder->current_char();
	while (*literal_str != '\0') {
		if (cur_char != *literal_str) {
			throw input_error(input_error::error_code::_literal_error, _line, _col);
		}
		cur_char = _decoder->next_char();
		*literal_str++;
	}
	if (cur_char != std::char_traits<char>::eof() && cur_char != '\t' && cur_char != ' ' &&
		cur_char != ']' && cur_char != '}' && cur_char != '\n' && cur_char != '\r' && cur_char != ',') {
		throw input_error(input_error::error_code::_literal_error, _line, _col);
	}
	_decoder->push_buff(_decoder->current_char());
	return token(type);
}

json_input_processor::json_input_processor() : base_input_processor() {}

json_input_processor::json_input_processor(const json_input_processor & jip) : base_input_processor(jip) {}

token json_input_processor::next_token(i_decoder_ptr_ref _decoder){
	_decoder->next_char();
	_scs_fsm.proc(_decoder, _line, _col);
	char c = _decoder->current_char();
	_col++;
	switch (c){
	case std::char_traits<char>::eof(): return token(token_type::_end);
	case '{': return token(token_type::_open_curly_brt);
	case '}': return token(token_type::_close_curly_brt);
	case '[': return token(token_type::_open_square_brt);
	case ']': return token(token_type::_close_square_brt);
	case ':': return token(token_type::_colon);
	case ',': return token(token_type::_comma);
	case '\"': return parse_string(_decoder, '\"');
	case '-':
	case '0': case '1': case '2': case '3': case '4': 
	case '5': case '6': case '7': case '8': case '9':
		return parse_number(_decoder);
	case 't':
		return parse_literal(_decoder, "true", token_type::_true);
	case 'f':
		return parse_literal(_decoder, "false", token_type::_false);
	case 'n':
		return parse_literal(_decoder, "null", token_type::_null);
	default: throw input_error(input_error::error_code::_invalid_token, _line, _col);
	}
}

json5_input_processor::json5_input_processor() : base_input_processor() {}

json5_input_processor::json5_input_processor(const json_input_processor & jip) : base_input_processor(jip) {}

token json5_input_processor::next_token(i_decoder_ptr_ref _decoder){
	_decoder->next_char();
	_scs_fsm.proc(_decoder, _line, _col);
	char cur_char = _decoder->current_char();
	_col++;
	if (cur_char == '+' || cur_char == '-') {
		return parse_json5_number(_decoder);
	}
	if (std::isalpha(cur_char) || cur_char == '_' || cur_char == '&') {
		return parse_literal_or_indentifier(_decoder);
	}
	switch (cur_char) {
	case std::char_traits<char>::eof(): 
		return token(token_type::_end);
	case '{': return token(token_type::_open_curly_brt);
	case '}': return token(token_type::_close_curly_brt);
	case '[': return token(token_type::_open_square_brt);
	case ']': return token(token_type::_close_square_brt);
	case ':': return token(token_type::_colon);
	case ',': return token(token_type::_comma);
	case '\"': return parse_string(_decoder, '\"');
	case '\'': return parse_string(_decoder, '\'');
	case '0': return parse_digit_or_hex(_decoder);
	case '.': case '1': case '2': case '3':
	case '4': case '5': case '6': 
	case '7': case '8': case '9': 
		return parse_number(_decoder);
	default: throw input_error(input_error::error_code::_invalid_token, _line, _col);
	};
}

token json5_input_processor::parse_digit_or_hex(i_decoder_ptr_ref _decoder) {
	char cur_char = _decoder->next_char();
	if (cur_char == 'x' || cur_char == 'X') {
		auto res = _hex_fsm.run(_decoder, _line, _col);
		if (res.second) {
			return token(res.first);
		}
		throw input_error(input_error::error_code::_invalis_hex_number, _line, _col);
	}
	return parse_number(_decoder);
}

token json5_input_processor::parse_infinity(i_decoder_ptr_ref _decoder, bool _negative){
	_decoder->next_char();
	token result = parse_literal(_decoder, "nfinity", token_type::_number);
	if (_negative) {
		result.double_data(-std::numeric_limits<double>::infinity());
	}
	else {
		result.double_data(std::numeric_limits<double>::infinity());
	}
	return result;
}

token json5_input_processor::parse_nan(i_decoder_ptr_ref _decoder, bool _negative){
	token result = parse_literal(_decoder, "NaN", token_type::_number);
	if (_negative) {
		result.double_data(-std::numeric_limits<double>::quiet_NaN());
	}
	else {
		result.double_data(std::numeric_limits<double>::quiet_NaN());
	}
	return result;
}

token json5_input_processor::parse_literal_or_indentifier(i_decoder_ptr_ref _decoder){
	std::string indetifier_string;
	char cur_char = _decoder->current_char();
	while (std::isalnum(cur_char) || cur_char == '_' || cur_char == '&') {
		_col++;
		indetifier_string += cur_char;
		cur_char = _decoder->next_char();
	}
	_decoder->push_buff(cur_char);
	if (indetifier_string == "true") {
		return token(token_type::_true);
	}
	else if (indetifier_string == "false") {
		return token(token_type::_false);
	}
	else if(indetifier_string == "null") {
		return token(token_type::_null);
	}
	else if (indetifier_string =="Infinity") {
		return token(std::numeric_limits<double>::infinity());
	}
	else if (indetifier_string == "NaN") {
		return token(std::numeric_limits<double>::quiet_NaN());
	}
	return token(indetifier_string);
}

token json5_input_processor::parse_json5_number(i_decoder_ptr_ref _decoder){
	token result;
	bool is_negtive = false;
	if (_decoder->current_char() == '-'){
		is_negtive = true;
	}
	switch (_decoder->next_char()){
	case 'I':
	case 'i':
		return parse_infinity(_decoder, is_negtive);
	case 'N':
		return parse_nan(_decoder, is_negtive);
	case '0': 
		result = parse_digit_or_hex(_decoder); 
		break;
	case '.':
	case '1': case '2': case '3':
	case '4': case '5': case '6':
	case '7': case '8': case '9':
		result = parse_number(_decoder);
		break;
	default: throw input_error(input_error::error_code::_invalid_number, _line, _col);
	}
	if (is_negtive) {
		result.double_data(-result.double_data()); 
	}
	return result;
}

skip_coment_and_space_fsm::skip_coment_and_space_fsm() 
	: _state(skip_coment_and_space_fsm::state::_none){
}

void skip_coment_and_space_fsm::proc(i_decoder_ptr_ref decoder, size_t & line, size_t & col){
	while (_state != state::_end) {
		switch (_state) {
		case state::_none:
			none_proc(decoder, line, col);
			break;
		case state::_line_comment:
			line_comment_proc(decoder, line, col);;
			break;
		case state::_block_comment:
			block_comment_proc(decoder, line, col);
			break;
		}
	}
	_state = state::_none; // востанавливаем состояние машины
}

void skip_coment_and_space_fsm::none_proc(i_decoder_ptr_ref decoder, size_t & line, size_t & col){
	switch (decoder->current_char()){
	case std::char_traits<char>::eof():
		_state = state::_end;
		decoder->push_buff(std::char_traits<char>::eof());
		break;
	case ' ':
	case '\t':
		col++;
		decoder->next_char();
		break;
	case '\n':
		line++;
		col = 0;
		decoder->next_char();
		break;
	case '\r':
		col = 0;
		decoder->next_char();
		if(decoder->current_char() == '\n'){
			line++;
			decoder->next_char();
		}
		break;
	case '/':
		decoder->next_char();
		if (decoder->current_char() == '*') {
			_state = state::_block_comment;
			decoder->next_char();
		}
		else if (decoder->current_char() == '/') {
			_state = state::_line_comment;
			decoder->next_char();
		}
		else {
			_state = state::_end;
		}
		break;
	default:
		_state = state::_end;
		break;
	}
}

void skip_coment_and_space_fsm::line_comment_proc(i_decoder_ptr_ref decoder, size_t & line, size_t & col){
	switch (decoder->current_char()){
	case std::char_traits<char>::eof():
		_state = state::_end;
		break;
	case '\n':
	case '\r':
		_state = state::_none;
		break;
	default:
		decoder->next_char();
		break;
	}
}

void skip_coment_and_space_fsm::block_comment_proc(i_decoder_ptr_ref decoder, size_t & line, size_t & col){
	switch (decoder->current_char()){
	case '*':
		decoder->next_char();
		if (decoder->current_char() == '/') {
			decoder->next_char();
			_state = state::_none;
		}
		break;
	default:
		if (decoder->current_char() == '\n') {
			line++;
		}
		decoder->next_char();
		break;
	}
}

digit_parse_fsm::digit_parse_fsm() : _state(state::_none) { }

std::pair<double, bool> digit_parse_fsm::run(encodings::i_decoder_ptr_ref decoder, size_t & line, size_t & col){
	std::string _digit_string;
	bool valid = true;
	_state = table[(size_t)_state][char_type(decoder->current_char())];
	while (_state != state::_end) {
		switch (_state) {
		case json::io::fsm::digit_parse_fsm::state::_sing:
		case json::io::fsm::digit_parse_fsm::state::_digit:
		case json::io::fsm::digit_parse_fsm::state::_dot:
		case json::io::fsm::digit_parse_fsm::state::_fract:
		case json::io::fsm::digit_parse_fsm::state::_exp_s:
		case json::io::fsm::digit_parse_fsm::state::_exp:
			_digit_string += decoder->current_char();
			break;
		case json::io::fsm::digit_parse_fsm::state::_error:
			valid = false;
			break;
		}
		decoder->next_char();
		_state = table[(size_t)_state][char_type(decoder->current_char())];
	}
	decoder->push_buff(decoder->current_char());
	_state = state::_none;
	col += _digit_string.size();
	if (valid) {
		return std::make_pair(std::stod(_digit_string), true);
	}
	return std::make_pair(0, false);
}

size_t digit_parse_fsm::char_type(char c){
	switch (c){
	case '+':
	case '-':
		return 0;
	case '.': 
		return 1;
	case '0':
	case '1': case '2': case '3':
	case '4': case '5': case '6':
	case '7': case '8': case '9':
		return 2;
	case 'e':
	case 'E':
		return 3;
	case ']':
	case '}':
	case ' ':
	case '\r':
	case '\n':
	case '\0':
	case '\t':
	case ',':
	case std::char_traits<char>::eof():
		return 5;
	}
	return 4;
}

hex_parse_fsm::hex_parse_fsm() : _state(state::_zero) { }

std::pair<double, bool> hex_parse_fsm::run(encodings::i_decoder_ptr_ref decoder, size_t & line, size_t & col){
	std::string _digit_string = "0";
	bool valid = true;
	_state = table[(size_t)_state][char_type(decoder->current_char())];
	while (_state != state::_end) {
		switch (_state) {
		case json::io::fsm::hex_parse_fsm::state::_prefix:
		case json::io::fsm::hex_parse_fsm::state::_digit:
			_digit_string += decoder->current_char();
			break;
		case json::io::fsm::hex_parse_fsm::state::_error:
			valid = false;
			break;
		}
		decoder->next_char();
		_state = table[(size_t)_state][char_type(decoder->current_char())];
	}
	decoder->push_buff(decoder->current_char());
	_state = state::_zero;
	col += _digit_string.size();
	if (valid) {
		return std::make_pair(static_cast<double>(std::stoull(_digit_string, nullptr, 16)), true);
	}
	return std::make_pair(0, false);
}

size_t hex_parse_fsm::char_type(char c){
	switch (c) {
	case 'x':
	case 'X':
		return 0;
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case '0':
	case '1': case '2': case '3':
	case '4': case '5': case '6':
	case '7': case '8': case '9':
		return 1;
	case ']':
	case '}':
	case ' ':
	case '\t':
	case '\r':
	case '\n':
	case ',':
	case '\0':
	case std::char_traits<char>::eof():
		return 3;
	}
	return 2;
}
