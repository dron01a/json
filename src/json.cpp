#include "json.h"

json::file_reader::file_reader(const std::string & file_name){
	file.open(file_name, std::ios::binary);
}

json::file_reader::file_reader(const char * file_name){
	file.open(file_name, std::ios::binary);
}

json::file_reader::~file_reader(){
	if (file.good()) {
		file.close();
	}
}

char json::file_reader::get_next_char(){
	cur_char = file.get();
	if (file.eof()) {
		cur_char =  '\0';
	}
	return cur_char;
}

char & json::file_reader::get_last_char(){
	return cur_char;
}

void json::file_reader::step_back(int n){
	file.seekg(-n, std::ios::cur);
}

bool json::file_reader::ready(){
	return file.good() && file.is_open();
}

json::string_reader::string_reader(const std::string & string){
	str = string;
}

json::string_reader::string_reader(const char * string){
	str = string;
}

char json::string_reader::get_next_char(){
	if (position < str.size()) {
		return str[position++];
	}
	return '\0';
}

char & json::string_reader::get_last_char(){
	if (position < str.size() && position > 0) {
		return str[position - 1];
	}
	if (position == 0) {
		return str[0];
	}
	return str[str.size() - 1];
}

void json::string_reader::step_back(int n){
	if (position - n >= 0) {
		position -= n;
	}
}

bool json::string_reader::ready(){
	return !str.empty() && position < str.size();
}

json::tokenizer::tokenizer(i_reader * _reader){
	reader = std::move(_reader);
}

json::tokenizer::~tokenizer(){
	if (reader) {
		delete reader;
	}
}

json::token & json::tokenizer::get_next_token(){
	char cur_char = reader->get_next_char();
	while (cur_char == ' ' || cur_char == '\t') {
		cur_char = reader->get_next_char();
	}
	switch (cur_char){
	case '{':
		cur_token = create_token( token_type::_open_curly_brt );
		break;
	case '}':
		cur_token = create_token( token_type::_close_curly_brt );
		break;
	case '[':
		cur_token = create_token(token_type::_open_square_brt);
		break;
	case ']':
		cur_token = create_token(token_type::_close_square_brt);
		break;
	case '\"':
		cur_token = create_token(string_parse(), token_type::_string);
		break;
	case ':':
		cur_token = create_token(token_type::_colon);
		break;
	case ',':
		cur_token = create_token(token_type::_comma);
		break;
	case '-': case '1':case '2': case '3':case '4': case '5':
	case '6': case '7': case '8': case '9':
		cur_token = create_token(number_parse(), token_type::_number);
		break;
	case '\0':
		cur_token = create_token(token_type::_end);
		break;
	case 't':
		if (literal_parse("true")) {
			cur_token = create_token(token_type::_true);
		}
		break;
	case 'f':
		if (literal_parse("false")) {
			cur_token = create_token(token_type::_false);
		}
		break;
	case 'n':
		if (literal_parse("null")) {
			cur_token = create_token(token_type::_null);
		}
		break;
	default:
		throw error_type::_error_token;
	}
	return cur_token;
}

json::token & json::tokenizer::get_last_token(){
	return cur_token;
}

json::token json::tokenizer::create_token(token_type _t){
	return{ "", _t };
}

json::token json::tokenizer::create_token(std::string str, token_type _t){
	return { str, _t };
}

json::token json::tokenizer::create_token(char ch, token_type _t){
	return { std::string(1, ch), _t };
}

std::string json::tokenizer::string_parse(){
	std::string result;
	char cur_char = reader->get_next_char();
	while (cur_char != '\"') {
		result += cur_char;
		cur_char = reader->get_next_char();
	}
	return result;
}

std::string json::tokenizer::number_parse(){
	std::string result;
	if (reader->get_last_char() == '-') {
		result += reader->get_last_char();
		reader->get_next_char();
	}
	char cur_char = reader->get_last_char();
	while (std::isdigit(cur_char)) {
		result += cur_char;
		cur_char = reader->get_next_char();
		switch (cur_char){
		case 'E':
		case 'e':
			result += cur_char;
			cur_char = reader->get_next_char();
			if (cur_char == '+' || cur_char == '-') {
				result += cur_char;
				cur_char = reader->get_next_char();
			}
			break;
		case '.':
			result += cur_char;
			cur_char = reader->get_next_char();
			if (!std::isdigit(cur_char)) {
				throw error_type::_invalid_number_format;
			}
			result += cur_char;
			cur_char = reader->get_next_char();
			break;
		}
	}
	try {
		reader->step_back(1);
		std::stod(result.c_str());
	}
	catch (std::exception & e) {
		throw error_type::_invalid_number;
	}
	return result;
}

bool json::tokenizer::literal_parse(std::string _literal){
	char cur_char = reader->get_next_char();
	for (size_t i = 1; i < _literal.size(); ++i) {
		if (cur_char != _literal[i] || cur_char == '\0') {
			throw error_type::_literal_error;
		}
		cur_char = reader->get_next_char();
	}
	return true;
}
