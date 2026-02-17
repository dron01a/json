#include "input_processor.h"

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
	case input_error::error_code::_error_token:
		return "error token";
	case input_error::error_code::_invalid_string:
		return "invalid string";
	case input_error::error_code::_invalid_number:
		return "invalid number";
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
			result_string += _c;
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
	case 'u':
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

uint32_t base_input_processor::parse_unicode_pair(i_decoder_ptr_ref _decoder){
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

token base_input_processor::parse_number(i_decoder_ptr_ref _decoder){
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

token base_input_processor::parse_literal(i_decoder_ptr_ref _decoder, const char * literal_str, token_type type){
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

json_input_processor::json_input_processor() : base_input_processor() {}

json_input_processor::json_input_processor(const json_input_processor & jip) : base_input_processor(jip) {}

token json_input_processor::next_token(i_decoder_ptr_ref _decoder){
	_decoder->next_char();
	_scs_fsm.proc(_decoder, _line, _col);
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
	default: throw input_error(input_error::error_code::_error_token, _line, _col);
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
	case std::char_traits<char>::eof(): return token(token_type::_end);
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
	default: throw input_error(input_error::error_code::_error_token, _line, _col);
	};
}

token json5_input_processor::parse_digit_or_hex(i_decoder_ptr_ref _decoder) {
	char cur_char = _decoder->peek_char();
	if (cur_char == 'x' || cur_char == 'X') {
		_decoder->next_char(); // пропуск х
		return parse_hex_number(_decoder);
	}
	return parse_number(_decoder);
}

token json5_input_processor::parse_hex_number(i_decoder_ptr_ref _decoder){
	std::string hex = "0x";
	char cur_char = _decoder->next_char();
	while (std::isxdigit(cur_char)) {
		_col++;
		hex += cur_char;
		cur_char = _decoder->next_char();
	}
	_decoder->push_buff(cur_char);
	try {
		return token(static_cast<double>(std::stoull(hex, nullptr, 16)));
	}
	catch (std::exception & err) {
		throw input_error(input_error::error_code::_invalis_hex_number, _line, _col);
	}
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
