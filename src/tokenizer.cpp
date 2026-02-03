#include "tokenizer.h"

using namespace json::encodings;
using namespace json::io_base;
using namespace json::io::basic_processors;

json::io::token::token() : _type(token_type::_none) {}

json::io::token::token(const token & other) {
	copy_data(other);
}

json::io::token::token(token_type t) : _type(t) {}

json::io::token::token(double data) : _type(token_type::_number){
	_double_data = data;
}

json::io::token::token(const char * data) : _type(token_type::_string) {
	_string_data = std::make_unique<std::string>(data);
}

json::io::token::token(const std::string & data) : _type(token_type::_string) {
	_string_data = std::make_unique<std::string>(data);
}

json::io::token::~token(){
	if (_type == token_type::_string) {
		_string_data.~unique_ptr();
	}
}

json::io::token & json::io::token::operator=(const token & other){
	copy_data(other);
	return *this;
}

std::string json::io::token::string_data() const{
	if (_type == token_type::_string) {
		return *_string_data;
	}
	return "";
}

double json::io::token::double_data() const{
	if (_type == token_type::_number) {
		return _double_data;
	}
	return 0.0;
}

json::io::token_type json::io::token::type() const{
	return _type;
}

void json::io::token::type(token_type t){
	_type = t;
	switch (_type) {
	case token_type::_number:
		_double_data = 0;
		break;
	case token_type::_string:
		_string_data = std::make_unique<std::string>("");
		break;
	}
}

void json::io::token::copy_data(const token & other){
	_type = other._type;
	switch (other._type) {
	case token_type::_number:
		_double_data = other._double_data;
		break;
	case token_type::_string:
		_string_data = std::make_unique<std::string>(*other._string_data);
		break;
	}
}

json::io::tokenizer::tokenizer(std::unique_ptr<i_input_processor> input_proc, std::unique_ptr<i_input> input, size_t & line, size_t & col){
	_input_proc = std::move(input_proc);
	_input = std::move(input);
	_line =& line;
	_col = &col;
}

json::io::token & json::io::tokenizer::next(){
	skip_space();
	char cur_char = _input->last_char();
	switch (cur_char){
	case '{':
		cur_token = token(token_type::_open_curly_brt);
		break;
	case '}':
		cur_token = token(token_type::_close_curly_brt);
		break;
	case '[':
		cur_token = token(token_type::_open_square_brt);
		break;
	case ']':
		cur_token = token(token_type::_close_square_brt);
		break;
	case '\"':
		cur_token = token(_input_proc->string_processing());
		break;
	case ':':
		cur_token = token(token_type::_colon);
		break;
	case ',':
		cur_token = token(token_type::_comma);
		break;
	case '.' :case '-': case '0': case '1':case '2': case '3':case '4':
	case '5': case '6': case '7': case '8': case '9':
		cur_token = token(_input_proc->digit_processing());
		break;
	case '\0':
		cur_token = token(token_type::_end);
		break;
	case 't':
		if (!_input_proc->literal_processing("true")) {
			throw input_error(input_error::error_code::_literal_error, *_line, *_col);
		}
		cur_token = token(token_type::_true);
		break;
	case 'f':
		if (!_input_proc->literal_processing("false")) {
			throw input_error(input_error::error_code::_literal_error, *_line, *_col);
		}
		cur_token = token(token_type::_false);
		break;
	case 'n':
		if (!_input_proc->literal_processing("null")) {
			throw input_error(input_error::error_code::_literal_error, *_line, *_col);
		}
		cur_token = token(token_type::_null);
	default:
		throw input_error(input_error::error_code::_error_token, *_line, *_col);
	}
	return cur_token;
}

json::io::token & json::io::tokenizer::last(){
	return cur_token;
}

void json::io::tokenizer::skip_space() {
	char cur_char = _input->next_char();
	while (cur_char == ' ' || cur_char == '\t' || cur_char == '\n' || cur_char == '\r') {
		switch (cur_char) {
		case ' ':
		case '\t':
			*_col++;
			break;
		case '\n':
		case '\r':
			*_col = 0;
			*_line++;
			break;
		}
		cur_char = _input->next_char();
	}
}