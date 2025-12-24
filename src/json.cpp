#include "json.h"

json::file_reader::file_reader(const std::string & file_name){
	file.open(file_name, std::ios::binary);
}

json::file_reader::file_reader(const char * file_name){
	file.open(file_name, std::ios::in | std::ios::binary);
}

json::file_reader::~file_reader(){
	if (file.is_open()) {
		file.close();
	}
}

char json::file_reader::get_next_char(){
	cur_char = file.get();
	if (file.eof()) {
		cur_char = '\0';
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

bool json::tokenizer::ready() const{
	return reader->ready();
}

json::token & json::tokenizer::get_next_token(){
	skip_space();
	char cur_char = reader->get_last_char();
	col++;
	switch (cur_char){
	case '{':
		cur_token = token(token_type::_open_curly_brt);
		break;
	case '}':
		cur_token = token( token_type::_close_curly_brt );
		break;
	case '[':
		cur_token = token(token_type::_open_square_brt);
		break;
	case ']':
		cur_token = token(token_type::_close_square_brt);
		break;
	case '\"':
		cur_token.string_data = string_parse();
		cur_token._type = token_type::_string;
		break;
	case ':':
		cur_token = token(token_type::_colon);
		break;
	case ',':
		cur_token = token(token_type::_comma);
		break;
	case '-': case '1':case '2': case '3':case '4': case '5':
	case '6': case '7': case '8': case '9':
		cur_token.num_data = number_parse();
		cur_token._type = token_type::_number;
		break;
	case '\0':
		cur_token = token(token_type::_end);
		break;
	case 't':
		if (literal_parse("true")) {
			cur_token = token(token_type::_true);
		}
		break;
	case 'f':
		if (literal_parse("false")) {
			cur_token = token(token_type::_false);
		}
		break;
	case 'n':
		if (literal_parse("null")) {
			cur_token = token(token_type::_null);
		}
		break;
	default:
		throw error(col, str, error_type::_error_token);
	}
	return cur_token;
}

json::token & json::tokenizer::get_last_token(){
	return cur_token;
}

void json::tokenizer::skip_space(){
	char cur_char = reader->get_next_char();
	while (cur_char == ' ' || cur_char == '\t' || cur_char == '\n' || cur_char == '\r') {
		switch (cur_char) {
		case ' ':
		case '\t':
			col++;
			break;
		case '\n':
		case '\r':
			col = 0;
			str++;
			break;
		}
		cur_char = reader->get_next_char();
	}
}

std::string json::tokenizer::string_parse(){
	std::string result;
	char cur_char = reader->get_next_char();
	while (cur_char != '\"') {
		result += cur_char;
		cur_char = reader->get_next_char();
	}
	col += result.size();
	return result;
}

double json::tokenizer::number_parse(){
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
				throw error(col, str, error_type::_invalid_number_format);
			}
			result += cur_char;
			cur_char = reader->get_next_char();
			break;
		}
	}
	try {
		reader->step_back(1);
		col += result.size();
		return std::stod(result.c_str());
	}
	catch (std::exception & e) {
		throw error(col, str, error_type::_invalid_number);
	}
}

bool json::tokenizer::literal_parse(std::string _literal){
	char cur_char = reader->get_next_char();
	for (size_t i = 1; i < _literal.size(); ++i) {
		if (cur_char != _literal[i] || cur_char == '\0') {
			throw error(col, str, error_type::_literal_error);
		}
		cur_char = reader->get_next_char();
	}
	col += _literal.size();
	return true;
}

json::json_value::json_value() : _type(value_type::_null){}

json::json_value::json_value(bool data){
	bool_data = data; 
	_type = value_type::_bool;
}

json::json_value::json_value(double data){
	num_data = data; 
	_type = value_type::_number;
}

json::json_value::json_value(const std::string & string){
	str_data = new std::string(string);
	_type = value_type::_string;
}

json::json_value::json_value(const char * string){
	str_data = new std::string(string);
	_type = value_type::_string;
}

json::json_value::json_value(json_array data){
	array_data = new json_array(data);
	_type = value_type::_array;
}

json::json_value::json_value(json_object data){
	object_data = new json_object(data);
	_type = value_type::_object;
}

json::json_value::json_value(const json_value & val){
	copy_data(val);
}

json::json_value::json_value(json_value && val){
	move_data(std::move(val));
}

json::json_value & json::json_value::operator=(json_value & val){
	copy_data(val);
	return *this;
}

json::json_value & json::json_value::operator=(json_value && val){
	move_data(std::move(val));
	return *this;
}

json::json_value::~json_value(){
	clear_data();
}

bool & json::json_value::as_bool(){
	return bool_data;
}

void json::json_value::as_bool(bool & val){
	clear_data();
	_type = value_type::_bool;
	bool_data = val;
}

double & json::json_value::as_num(){
	return num_data;
}

void json::json_value::as_num(double num){
	clear_data();
	_type = value_type::_number;
	num_data = num;
}

std::string & json::json_value::as_string(){
	return *str_data;
}

void json::json_value::as_string(const std::string & string){
	clear_data();
	_type = value_type::_string;
	str_data = new std::string(string);
}

void json::json_value::as_string(const char * string){
	clear_data();
	_type = value_type::_string;
	str_data = new std::string(string);
}

json::json_array * json::json_value::as_array(){
	return array_data;
}

void json::json_value::as_array(json_array array){
	clear_data();
	_type = value_type::_array;
	array_data = new json_array(array);
}

json::json_object * json::json_value::as_object() {
	return object_data;
}

void json::json_value::as_object(json_object object){
	clear_data();
	_type = value_type::_object;
	object_data = new json_object(object);
}

json::json_value * json::json_value::find(const std::string & name){
	return find(name.c_str());
}

json::json_value * json::json_value::find(const char * name){
	json_value * result = nullptr;
	switch (_type) {
	case json::value_type::_array:
		result = find_in_array(name);
		break;
	case json::value_type::_object:
		result = find_in_object(name);
		break;
	}
	return result;
}

json::value_type json::json_value::type() const{
	return _type;
}

void json::json_value::type(value_type _t){
	_type = _t;
}

void json::json_value::clear_data(){
	switch (_type){
	case json::value_type::_string:
		delete str_data;
		str_data = nullptr;
		break;
	case json::value_type::_array:
		delete array_data;
		array_data = nullptr;
		break;
	case json::value_type::_object:
		delete object_data;
		object_data = nullptr;
		break;
	}
}

void json::json_value::copy_data(const json_value & val){
	clear_data();	
	_type = val._type;
	switch (_type) {
	case value_type::_bool:
		bool_data = val.bool_data;
		break;
	case value_type::_number:
		num_data = val.num_data;
		break;
	case value_type::_string:
		str_data = new std::string(*val.str_data);
		break;
	case value_type::_array:
		array_data = new json_array(*val.array_data);
		break;
	case value_type::_object:
		object_data = new json_object(*val.object_data);
		break;
	}
}

void json::json_value::move_data(json_value && val){
	clear_data();
	_type = std::move(val._type);
	switch (_type) {
	case value_type::_bool:
		bool_data = std::move(val.bool_data);
		break;
	case value_type::_number:
		num_data = std::move(val.num_data);
		break;
	case value_type::_string:
		std::swap(str_data, val.str_data);
		val.str_data = nullptr;
		break;
	case value_type::_array:
		std::swap(array_data, val.array_data);
		val.array_data = nullptr;
		break;
	case value_type::_object:
		std::swap(object_data, val.object_data);
		val.array_data = nullptr;
		break;
	}
}

json::json_value * json::json_value::find_in_array(const char * name){
	json::json_value * _result = nullptr;
	for (size_t _i = 0; _i < array_data->size(); ++_i) {
		switch (array_data->operator[](_i)._type){
		case json::value_type::_array:
			_result = array_data->operator[](_i).find_in_array(name);
			break;
		case json::value_type::_object:
			_result = array_data->operator[](_i).find_in_object(name);
			break;
		}
		if (_result) {
			break;
		}
	}
	return _result;
}

json::json_value * json::json_value::find_in_object(const char * name){
	json::json_value * _result = nullptr;
	if (object_data->count(std::string(name)) != 0) {
		_result = &object_data->operator[](name);
	}
	if (!_result) {
		for (auto it = object_data->begin(); it != object_data->end(); ++it) {
			switch (it->second._type) {
			case json::value_type::_array:
				_result = it->second.find_in_array(name);
				break;
			case json::value_type::_object:
				_result = it->second.find_in_object(name);
				break;
			}
			if (_result) {
				break;
			}
		}
	}
	return _result;
}

json::json_parser::json_parser() : _tokenizer(nullptr) {}

json::json_parser::~json_parser(){
	if (_tokenizer) {
		delete _tokenizer;
	}
}

json::json_value json::json_parser::load_from_file(const std::string & file_name){
	return load_from_string(file_name.c_str());
}

json::json_value json::json_parser::load_from_file(const char * file_name){
	if (_tokenizer) {
		delete _tokenizer;
	}
	_tokenizer = new tokenizer(new file_reader(file_name));
	if (_tokenizer->ready()) {
		return parse();
	}
	throw error(0, 0, error_type::_file_not_found);
}

json::json_value json::json_parser::load_from_string(const std::string & json_string){
	return load_from_string(json_string.c_str());
}

json::json_value json::json_parser::load_from_string(const char * json_string){
	if (_tokenizer) {
		delete _tokenizer;
	}
	_tokenizer = new tokenizer(new string_reader(json_string));
	if (_tokenizer->ready()) {
		return parse();
	}
	throw error(0, 0, error_type::_string_is_empty);
}

json::error json::json_parser::get_last_error(){
	return last_error;
}

json::json_value json::json_parser::parse(){
	json_value _result;
	try {
		token cur_token = _tokenizer->get_next_token();
		switch (cur_token._type) {
		case token_type::_open_curly_brt:
			_result = parse_object();
			break;
		case token_type::_open_square_brt:
			_result = parse_array();
			break;
		}
	}
	catch (error & e) {
		last_error = e;
	}
	return _result;
}

json::json_value json::json_parser::parse_json_value(){
	token cur_token = _tokenizer->get_last_token();
	switch (cur_token._type) {
	case token_type::_open_curly_brt:
		return parse_object();
	case token_type::_open_square_brt:
		return parse_array();
	case token_type::_null:
		return json_value();
	case token_type::_number:
		return json_value(cur_token.num_data);
	case token_type::_string:
		return json_value(cur_token.string_data.c_str());
	case token_type::_true:
		return json_value(true);
	case token_type::_false:
		return json_value(false);
	}
	throw error(_tokenizer->col, _tokenizer->str, error_type::_invalid_value);
}

json::json_value json::json_parser::parse_array(){
	json_value result; 
	result.as_array({});
	bool _parse = true;
	_tokenizer->get_next_token();
	while (_parse && _tokenizer->get_last_token()._type != token_type::_close_square_brt) {
		result.as_array()->push_back(parse_json_value());
		_tokenizer->get_next_token();
		switch (_tokenizer->get_last_token()._type){
		case token_type::_comma:
			_tokenizer->get_next_token();
			break;
		case token_type::_close_square_brt:
			_parse = false;
			break;
		default:
			throw error(_tokenizer->col, _tokenizer->str, error_type::_invalid_array_value);
		}
	}
	if (!_parse && _tokenizer->get_last_token()._type != token_type::_close_square_brt) {
		throw error(_tokenizer->col, _tokenizer->str, error_type::_invalid_array_value);
	}
	return result;
}

json::json_value json::json_parser::parse_object(){
	json_value result;
	result.as_object({});
	_tokenizer->get_next_token();
	while (_tokenizer->get_last_token()._type != token_type::_close_curly_brt) {
		if (_tokenizer->get_last_token()._type == token_type::_string) {
			std::string key = _tokenizer->get_last_token().string_data;
			_tokenizer->get_next_token();
			if (_tokenizer->get_last_token()._type != token_type::_colon) {
				throw error(_tokenizer->col, _tokenizer->str, error_type::_invalid_object);
			}
			_tokenizer->get_next_token();
			json_value val = parse_json_value();
			result.as_object()->insert({std::move(key), std::move(val)});
			_tokenizer->get_next_token();
			continue;
		}
		if (_tokenizer->get_last_token()._type == token_type::_comma) {
			_tokenizer->get_next_token();
			continue;
		}
		throw error(_tokenizer->col, _tokenizer->str, error_type::_error_token);
	}
	return result;
}
