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
	last_char = file.get();
	if (file.eof()) {
		last_char =  '\0';
	}
	return last_char;
}

char & json::file_reader::get_last_char(){
	return last_char;
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

json::token & json::tokenizer::get_nex_token(){
	char ch = reader->get_next_char();
	while (ch == ' ' || ch == '\t') {
		ch = reader->get_next_char();
	}
	switch (ch){
	case '{':
		cur_token = { std::string(1, ch), token_type::_open_curly_brt }; 
		break;
	case '}':
		cur_token = { std::string(1, ch), token_type::_close_curly_brt };
		break;
	case '[':
		cur_token = { std::string(1, ch), token_type::_open_square_brt };
		break;
	case ']':
		cur_token = { std::string(1, ch), token_type::_close_square_brt };
		break;
	case '\"':
		cur_token.data = string_parse();
		cur_token._type = token_type::_string;
		break;
	case ':':
		cur_token = { std::string(1, ch), token_type::_colon };
		break;
	case ',':
		cur_token = { std::string(1, ch), token_type::_comma };
		break;
	case '\t':
	case ' ':
		cur_token = get_nex_token();
		break;
	case '-': case '1':case '2': case '3':case '4': case '5':
	case '6': case '7': case '8': case '9':
		cur_token.data = number_parse();
		cur_token._type = token_type::_number;
		break;
	case '\0':
		cur_token = { std::string(1, ch), token_type::_end };
		break;
	default:
		throw error_type::_error_token;
		break;
	}
	return cur_token;
}

json::token & json::tokenizer::get_last_token(){
	return cur_token;
}

std::string json::tokenizer::string_parse(){
	std::string result;
	char ch = reader->get_next_char();
	while (ch != '\"') {
		result += ch;
		ch = reader->get_next_char();
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
				throw  error_type::_invalid_number_format;
			}
			result += cur_char;
			cur_char = reader->get_next_char();
			break;
		}
	}
	try {
		std::stod(result.c_str());
	}
	catch (std::exception & e) {
		throw error_type::_invalid_number;
	}
	return result;
}
