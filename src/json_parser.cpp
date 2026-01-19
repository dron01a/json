#include "json_parser.h"

#include <cctype>
#include <cstdio>

json::tokenizer::tokenizer(i_reader * _reader) {
	reader = std::move(_reader);
}

json::tokenizer::~tokenizer() {
	if (reader) {
		delete reader;
	}
}

bool json::tokenizer::ready() const {
	return reader->ready();
}

json::token & json::tokenizer::get_next_token() {
	skip_space();
	char cur_char = reader->get_last_char();
	col++;
	switch (cur_char) {
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
		cur_token.string_data = string_parse();
		cur_token._type = token_type::_string;
		break;
	case ':':
		cur_token = token(token_type::_colon);
		break;
	case ',':
		cur_token = token(token_type::_comma);
		break;
	case '-':case '0': case '1':case '2': case '3':case '4': case '5':
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

json::token & json::tokenizer::get_last_token() {
	return cur_token;
}

void json::tokenizer::skip_space() {
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

std::string json::tokenizer::string_parse() {
	std::string result;
	char cur_char = reader->get_next_char();
	while (cur_char != '\"') {
		switch (cur_char) {
		case '\0':
			throw error(col, str, error_type::_invalid_string);
		case '\\':
		{
			string_coder coder;
			result += coder.decode(reader, str, col);
		}
		break;
		default:
			result += cur_char;
		}
		cur_char = reader->get_next_char();
	}
	col += result.size();
	return result;
}

double json::tokenizer::number_parse() {
	std::string result;
	char cur_char = reader->get_last_char();
	if (cur_char == '-') {
		result += cur_char;
		cur_char = reader->get_next_char();
		if (!std::isdigit(cur_char) && cur_char != '.') {
			throw error(col, str, error_type::_invalid_number);
		}
	}
	while (std::isdigit(cur_char)) {
		result += cur_char;
		cur_char = reader->get_next_char();
	}
	if (cur_char == '.') {
		result += cur_char;
		cur_char = reader->get_next_char();
		while (std::isdigit(cur_char)) {
			result += cur_char;
			cur_char = reader->get_next_char();
		}
	}
	if (cur_char == 'e' || cur_char == 'E') {
		result += cur_char;
		cur_char = reader->get_next_char();
		if (cur_char == '+' || cur_char == '-') {
			result += cur_char;
			cur_char = reader->get_next_char();
		}
		while (std::isdigit(cur_char)) {
			result += cur_char;
			cur_char = reader->get_next_char();
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

bool json::tokenizer::literal_parse(std::string _literal) {
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

json::json_parser::json_parser() : _tokenizer(nullptr) {}

json::json_parser::~json_parser() {
	if (_tokenizer) {
		delete _tokenizer;
	}
}

json::json_value json::json_parser::load_from_file(const std::string & file_name) {
	return load_from_string(file_name.c_str());
}

json::json_value json::json_parser::load_from_file(const char * file_name) {
	if (_tokenizer) {
		delete _tokenizer;
	}
	_tokenizer = new tokenizer(new file_reader(file_name));
	if (_tokenizer->ready()) {
		return parse();
	}
	throw error(0, 0, error_type::_file_not_found);
}

json::json_value json::json_parser::load_from_string(const std::string & json_string) {
	return load_from_string(json_string.c_str());
}

json::json_value json::json_parser::load_from_string(const char * json_string) {
	if (_tokenizer) {
		delete _tokenizer;
	}
	_tokenizer = new tokenizer(new string_reader(json_string));
	if (_tokenizer->ready()) {
		return parse();
	}
	throw error(0, 0, error_type::_string_is_empty);
}

json::json_value json::json_parser::load_from_stream(std::istream & stream) {
	if (_tokenizer) {
		delete _tokenizer;
	}
	_tokenizer = new tokenizer(new stream_reader(stream));
	if (_tokenizer->ready()) {
		return parse();
	}
	throw error(0, 0, error_type::_stream_is_bad);
}

json::error json::json_parser::get_last_error() {
	return last_error;
}

json::json_value json::json_parser::parse() {
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

json::json_value json::json_parser::parse_json_value() {
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

json::json_value json::json_parser::parse_array() {
	json_value result;
	result.as_array({});
	bool _parse = true;
	_tokenizer->get_next_token();
	while (_parse && _tokenizer->get_last_token()._type != token_type::_close_square_brt) {
		result.as_array()->push_back(parse_json_value());
		_tokenizer->get_next_token();
		switch (_tokenizer->get_last_token()._type) {
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

json::json_value json::json_parser::parse_object() {
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
			result.as_object()->insert({ std::move(key), std::move(val) });
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