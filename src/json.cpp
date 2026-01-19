#include "json.h"

std::string json::string_coder::decode(i_reader * reader, size_t & line, size_t & col) {
	char cur_char = reader->get_next_char();
	col++;
	switch (cur_char) {
	case '"':
		return std::string(1, '\"');
	case '\\':
		return std::string(1, '\\');
	case '/':
		return std::string(1, '/');
	case 'b':
		return std::string(1, '\b');
	case 'f':
		return std::string(1, '\f');
	case 'n':
		return std::string(1, '\n');
	case 'r':
		return std::string(1, '\r');
	case 't':
		return std::string(1, '\t');
	case 'u':
		return utf32_to_utf8(read_unicode(reader, line, col));
	case '\0':
		throw error(col, line, error_type::_invalid_string);
	default:
		throw error(col, line, error_type::_invalid_escape);
	}
}

std::string json::string_coder::encode(const std::string & src_string) {
	std::ostringstream result;
	for (size_t i = 0; i < src_string.size(); ++i) {
		uint8_t c = static_cast<uint8_t>(src_string[i]);
		switch (c) {
		case '\"':
			result << "\\\"";
			break;
		case '\\':
			result << "\\\\";
			break;
		case '/':
			result << "\\/";
			break;
		case '\b':
			result << "\\b";
			break;
		case '\f':
			result << "\\f";
			break;
		case '\n':
			result << "\\n";
			break;
		case '\r':
			result << "\\r";
			break;
		case '\t':
			result << "\\t";
			break;
		default:
			if (c < 0x20 || c == 0x7F) {
				result << unicode_escape(c);
			}
			else {
				size_t len = get_utf8_len(c);
				for (size_t j = 0; j < len && i < src_string.size(); ++j) {
					result << src_string[i++];
				}
				i--;
			}
			break;
		}
	}
	return result.str();
}

char32_t json::string_coder::read_unicode(i_reader * reader, size_t & line, size_t & col) {
	std::string result;
	char cur_char;
	for (size_t i = 0; i < 4; ++i) {
		cur_char = reader->get_next_char();
		col++;
		if (cur_char == '\0') {
			throw error(col, line, error_type::_invalid_string);
		}
		if (!std::isxdigit(static_cast<uint8_t>(cur_char))) {
			throw error(col, line, error_type::_invalid_unicode_char);
		}
		result += cur_char;
	}
	try {
		uint32_t code = std::stoul(result, nullptr, 16);
		if (code > 0x10FFFF) {
			throw error(col, line, error_type::_invalid_unicode);
		}
		if (code >= 0xD800 && code <= 0xDBFF) {
			char32_t low_pair = read_low_pair(reader, line, col);
			return 0x10000 + ((code - 0xD800) << 10) + (low_pair - 0xDC00);
		}
		return static_cast<char32_t>(code);
	}
	catch (std::exception & e) {
		throw error(col, line, error_type::_invalid_unicode);
	}
}

char32_t json::string_coder::read_low_pair(i_reader * reader, size_t & line, size_t & col) {
	char cur_char = reader->get_next_char();
	col++;
	if (cur_char != '\\') {
		throw error(col, line, error_type::_invalid_unicode_low_pair);
	}
	cur_char = reader->get_next_char();
	col++;
	if (cur_char == 'u') {
		std::string hec_str;
		for (size_t i = 0; i < 4; ++i) {
			cur_char = reader->get_next_char();
			col++;
			if (cur_char == '\0') {
				throw error(col, line, error_type::_invalid_string);
			}
			if (!std::isxdigit(static_cast<uint8_t>(cur_char))) {
				throw error(col, line, error_type::_invalid_unicode_char);
			}
			hec_str += cur_char;
		}
		uint32_t code = std::stoul(hec_str, nullptr, 16);
		if (code >= 0xD800 && code <= 0xD8FF) {
			throw error(col, line, error_type::_invalid_unicode);
		}
		return code;
	}
	throw error(col, line, error_type::_invalid_unicode_low_pair);
}

std::string json::string_coder::utf32_to_utf8(char32_t code) {
	std::string result;
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

std::string json::string_coder::unicode_escape(uint32_t code) {
	std::ostringstream result;
	if (code <= 0xFFFF) {
		result << "\\u" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << code;
	}
	else if (code <= 0x10FFFF) {
		code -= 0x10000;
		uint16_t high = 0xD800 + (code >> 10);
		uint16_t low = 0xDC00 + (code & 0x3FF);
		result << "\\u" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << high
			<< "\\u" << std::setw(4) << low;
	}
	else {
		result << "\\uFFFD";
	}
	return result.str();;
}

size_t json::string_coder::get_utf8_len(unsigned char fb) {
	if ((fb & 0x80) == 0x00) {
		return 1;
	}
	if ((fb & 0xE0) == 0xC0) {
		return 2;
	}
	if ((fb & 0xF0) == 0xE0) {
		return 3;
	}
	if ((fb & 0xF8) == 0xF0) {
		return 4;
	}
	return 1;
}

uint32_t json::string_coder::decode_utf8(const std::string & str, size_t & position) {
	uint8_t f = static_cast<uint8_t>(str[position]);
	if (f < 0x80) {
		return f;
	}
	if ((f & 0xE0) == 0xE0) {
		if (position + 1 >= str.size()) {
			return 0xFFFD;
		}
		uint8_t s = static_cast<uint8_t>(str[position + 1]);
		if ((s & 0xC8) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (f & 0x1F) << 6 | (s & 0x3F);
		position += 1;
		return code;
	}
	if ((f & 0xE0) == 0xE0) {
		if (position + 2 >= str.size()) {
			return 0xFFFD;
		}
		uint8_t s = static_cast<uint8_t>(str[position + 1]);
		uint8_t t = static_cast<uint8_t>(str[position + 2]);
		if ((s & 0xC8) != 0x80 || (t & 0xC8) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (f & 0x0F) << 12 | (s & 0x3F) << 6 | (t & 0x3F);
		position += 2;
		return code;
	}
	if ((f & 0xE0) == 0xF0) {
		if (position + 3 >= str.size()) {
			return 0xFFFD;
		}
		uint8_t s = static_cast<uint8_t>(str[position + 1]);
		uint8_t t = static_cast<uint8_t>(str[position + 2]);
		uint8_t fo = static_cast<uint8_t>(str[position + 3]);
		if ((s & 0xC8) != 0x80 || (t & 0xC8) != 0x80 || (fo & 0xC8) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (f & 0x07) << 18 | (s & 0x3F) << 12 | (t & 0x3F) << 6 | (fo & 0x3F);
		position += 3;
		return code;
	}
	return 0xFFFD;
}

json::file_reader::file_reader(const std::string & file_name) {
	file.open(file_name, std::ios::binary);
}

json::file_reader::file_reader(const char * file_name) {
	file.open(file_name, std::ios::in | std::ios::binary);
}

json::file_reader::~file_reader() {
	if (file.is_open()) {
		file.close();
	}
}

char json::file_reader::get_next_char() {
	cur_char = file.get();
	if (file.eof()) {
		cur_char = '\0';
	}
	return cur_char;
}

char & json::file_reader::get_last_char() {
	return cur_char;
}

void json::file_reader::step_back(int n) {
	file.seekg(-n, std::ios::cur);
}

bool json::file_reader::ready() {
	return file.good() && file.is_open();
}

json::string_reader::string_reader(const std::string & string) {
	str = string;
}

json::string_reader::string_reader(const char * string) {
	str = string;
}

char json::string_reader::get_next_char() {
	if (position < str.size()) {
		return str[position++];
	}
	return '\0';
}

char & json::string_reader::get_last_char() {
	if (position < str.size() && position > 0) {
		return str[position - 1];
	}
	if (position == 0) {
		return str[0];
	}
	return str[str.size() - 1];
}

void json::string_reader::step_back(int n) {
	if (position - n >= 0) {
		position -= n;
	}
}

bool json::string_reader::ready() {
	return !str.empty() && position < str.size();
}

json::stream_reader::stream_reader(std::istream & stream) {
	this->stream = &stream;
}

json::stream_reader::~stream_reader() {
	stream = nullptr;
}

char json::stream_reader::get_next_char() {
	*stream >> cur_char;
	if (stream->eof()) {
		cur_char = '\0';
	}
	return cur_char;
}

char & json::stream_reader::get_last_char() {
	return cur_char;
}

void json::stream_reader::step_back(int n) {
	stream->seekg(-n, std::ios::cur);
}

bool json::stream_reader::ready() {
	return stream->good();
}

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

json::json_value::json_value() : _type(value_type::_null) {}

json::json_value::json_value(bool data) {
	bool_data = data;
	_type = value_type::_bool;
}

json::json_value::json_value(double data) {
	num_data = data;
	_type = value_type::_number;
}

json::json_value::json_value(const std::string & string) {
	str_data = new std::string(string);
	_type = value_type::_string;
}

json::json_value::json_value(const char * string) {
	str_data = new std::string(string);
	_type = value_type::_string;
}

json::json_value::json_value(json_array data) {
	array_data = new json_array(data);
	_type = value_type::_array;
}

json::json_value::json_value(json_object data) {
	object_data = new json_object(data);
	_type = value_type::_object;
}

json::json_value::json_value(const json_value & val) {
	copy_data(val);
}

json::json_value::json_value(json_value && val) {
	move_data(std::move(val));
}

json::json_value::json_value(const char * name, const json_value & val) {
	object_data = new json_object();
	object_data->insert({ name, val });
	_type = value_type::_object;
}

json::json_value::json_value(const char * name, const json_value && val) {
	object_data = new json_object();
	object_data->insert({ name, std::move(val) });
	_type = value_type::_object;
}

json::json_value & json::json_value::operator=(const json_value & val) {
	copy_data(val);
	return *this;
}

json::json_value & json::json_value::operator=(json_value && val) {
	move_data(std::move(val));
	return *this;
}

json::json_value::~json_value() {
	clear_data();
}

bool & json::json_value::as_bool() {
	return bool_data;
}

bool & json::json_value::as_bool() const {
	return const_cast<json_value*>(this)->as_bool();
}

void json::json_value::as_bool(bool & val) {
	clear_data();
	_type = value_type::_bool;
	bool_data = val;
}

double & json::json_value::as_num() {
	return num_data;
}

double & json::json_value::as_num() const {
	return const_cast<json_value*>(this)->as_num();
}

void json::json_value::as_num(double num) {
	clear_data();
	_type = value_type::_number;
	num_data = num;
}

std::string & json::json_value::as_string() {
	return *str_data;
}

std::string & json::json_value::as_string() const {
	return const_cast<json_value*>(this)->as_string();
}

void json::json_value::as_string(const std::string & string) {
	clear_data();
	_type = value_type::_string;
	str_data = new std::string(string);
}

void json::json_value::as_string(const char * string) {
	clear_data();
	_type = value_type::_string;
	str_data = new std::string(string);
}

json::json_array * json::json_value::as_array() {
	return array_data;
}

json::json_array * json::json_value::as_array() const {
	return const_cast<json_value*>(this)->as_array();
}

void json::json_value::as_array(json_array array) {
	clear_data();
	_type = value_type::_array;
	array_data = new json_array(array);
}

json::json_object * json::json_value::as_object() {
	return object_data;
}

json::json_object * json::json_value::as_object() const {
	return const_cast<json_value*>(this)->as_object();
}

void json::json_value::as_object(json_object object) {
	clear_data();
	_type = value_type::_object;
	object_data = new json_object(object);
}

json::json_value * json::json_value::find(const std::string & name) {
	return find(name.c_str());
}

json::json_value * json::json_value::find(const char * name) {
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

json::json_value * json::json_value::add(const json_value & val) {
	json_value * result = nullptr;
	switch (_type) {
	case json::value_type::_array:
		array_data->push_back(val);
		result = &array_data->back();
		break;
	}
	return result;
}

json::json_value * json::json_value::add(json_value && val) {
	json_value * result = nullptr;
	switch (_type) {
	case json::value_type::_array:
		array_data->push_back(std::move(val));
		result = &array_data->back();
		break;
	}
	return result;
}

json::json_value * json::json_value::add(const char * name, const json_value & val) {
	json_value * result = nullptr;
	switch (_type) {
	case json::value_type::_object:
		object_data->insert_or_assign(name, val);
		result = &object_data->operator[](name);
		break;
	case json::value_type::_array:
		array_data->push_back(json_value(name, val));
		result = &array_data->back();
		break;
	}
	return result;
}

json::json_value * json::json_value::add(const char * name, json_value && val) {
	json_value * result = nullptr;
	switch (_type) {
	case json::value_type::_object:
		object_data->insert_or_assign(name, std::move(val));
		result = &object_data->operator[](name);
		break;
	case json::value_type::_array:
		array_data->push_back(json_value(name, std::move(val)));
		result = &array_data->back();
		break;
	}
	return result;
}

json::json_value * json::json_value::add(const std::string & name, const json_value & val) {
	return add(name.c_str(), val);
}

json::json_value * json::json_value::add(const std::string & name, json_value && val) {
	return add(name.c_str(), std::move(val));
}

json::value_type json::json_value::type() const {
	return _type;
}

void json::json_value::type(value_type _t) {
	_type = _t;
}

void json::json_value::clear_data() {
	switch (_type) {
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

void json::json_value::copy_data(const json_value & val) {
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

void json::json_value::move_data(json_value && val) {
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

json::json_value * json::json_value::find_in_array(const char * name) {
	json::json_value * _result = nullptr;
	for (size_t _i = 0; _i < array_data->size(); ++_i) {
		switch (array_data->operator[](_i)._type) {
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

json::json_value * json::json_value::find_in_object(const char * name) {
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

json::string_writer::string_writer(std::string & _data) {
	_desc = &_data;
}

void json::string_writer::write_data(const char * data) {
	*_desc += data;
}

bool json::string_writer::ready() {
	return true;
}

json::file_writer::file_writer(const std::string & file_name) {
	_desc.open(file_name, std::ios::out | std::ios::binary);
}

json::file_writer::file_writer(const char * file_name) {
	_desc.open(file_name, std::ios::out | std::ios::binary);
}

json::file_writer::~file_writer() {
	_desc.close();
}

void json::file_writer::write_data(const char * data) {
	_desc << data;
}

bool json::file_writer::ready() {
	return _desc.good();
}

json::stream_writer::stream_writer(std::ostream & _data) {
	_desc = &_data;
}

json::stream_writer::~stream_writer() {
	_desc = nullptr;
}

void json::stream_writer::write_data(const char * data) {
	*_desc << data;
}

bool json::stream_writer::ready() {
	return _desc->good();
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

json::write_config json::compact() {
	write_config conf;
	conf.reset();
	return conf;
}

json::write_config json::format() {
	write_config conf;
	conf.set_flag(write_flags::format);
	conf.space_count(1);
	conf.indent("\t");
	return conf;
}

json::write_config::write_config() : _flags(0) {}

void json::write_config::set_flag(write_flags flag) {
	this->_flags |= (uint32_t)flag;
}

void json::write_config::reset_flag(write_flags flag) {
	_flags ^= (uint32_t)flag & (_flags & (uint32_t)flag);
}

void json::write_config::reset() {
	_flags = 0;
}

bool json::write_config::has_flag(write_flags flag) {
	return (_flags & (uint32_t)flag) != 0;
}

void json::write_config::indent(const char * str) {
	_indent = str;
}

std::string & json::write_config::indent() {
	return _indent;
}

std::string json::write_config::indent() const {
	return _indent;
}

void json::write_config::space(const char * str) {
	_space = str;
}

std::string & json::write_config::space() {
	return _space;
}

std::string json::write_config::space() const {
	return _space;
}

void json::write_config::space_count(size_t n) {
	_space_count = n;
}

size_t & json::write_config::space_count() {
	return _space_count;
}

size_t json::write_config::space_count() const {
	return _space_count;
}

void json::write_config::new_line(const char * str) {
	_new_line = str;
}

std::string & json::write_config::new_line() {
	return _new_line;
}

std::string json::write_config::new_line() const {
	return _new_line;
}

void json::write_config::presition(size_t n) {
	_presit = n;
}

size_t & json::write_config::presition() {
	return _presit;
}

size_t json::write_config::presition() const {
	return _presit;
}

json::json_writer::json_writer(write_config conf) : writer(nullptr) , _config(conf) {
	if (_config.has_flag(write_flags::using_tabs)) {
		_config.space("\t");
	}
}

void json::json_writer::write_to_file(const json_value & json_val, const std::string & file_name) {
	write_to_file(json_val, file_name.c_str());
}

void json::json_writer::write_to_file(const json_value & json_val, const char * file_name) {
	writer = new json::file_writer(file_name);
	if (!writer->ready()) {
		throw error(0, 0, error_type::_file_not_found);
	}
	if (_config.has_flag(write_flags::using_tabs)) {
		_config.space("\t");
	}
	write(json_val);
	delete writer;
}

void json::json_writer::write_to_string(const json_value & json_val, std::string & json_string) {
	writer = new json::string_writer(json_string);
	if (_config.has_flag(write_flags::using_tabs)) {
		_config.space("\t");
	}
	write(json_val);
	delete writer;
}

void json::json_writer::write_to_stream(const json_value & json_val, std::ostream & stream) {
	writer = new json::stream_writer(stream);
	if (!writer->ready()) {
		throw error(0, 0, error_type::_stream_is_bad);
	}
	if (_config.has_flag(write_flags::using_tabs)) {
		_config.space("\t");
	}
	write(json_val);
	delete writer;
}

void json::json_writer::config(write_config conf){
	_config = conf;
}

json::write_config & json::json_writer::config(){
	return _config;
}

json::write_config json::json_writer::config() const{
	return _config;
}

void json::json_writer::write_indent(){
	if (!_config.has_flag(write_flags::format)) {
		return;
	}
	writer->write_data(_config.new_line().c_str());
	for (size_t i = 0; i < indent_level; ++i) {
		writer->write_data(_config.indent().c_str());
	}
}

void json::json_writer::write_space(){
	if (!_config.has_flag(write_flags::format)) {
		return;
	}
	for (size_t i = 0; i < _config.space_count(); ++i) {
		writer->write_data(_config.space().c_str());
	}
}

void json::json_writer::write(const json_value & json_val) {
	switch (json_val.type()) {
	case value_type::_null:
		writer->write_data("null");
		break;
	case value_type::_bool:
		write_bool(json_val.as_bool());
		break;
	case value_type::_number:
		write_digit(json_val.as_num());
		break;
	case value_type::_string:
		write_string(json_val.as_string());
		break;
	case value_type::_array:
		write_array(*json_val.as_array());
		break;
	case value_type::_object:
		write_object(*json_val.as_object());
		break;
	}
}

void json::json_writer::write_bool(const bool & data) {
	if (!_config.has_flag(write_flags::bool_as_num)) {
		writer->write_data((data ? "true" : "false"));
	}
	else {
		writer->write_data((data ? "0" : "1"));
	}
}

void json::json_writer::write_digit(const double & data) {
	double mult = std::pow(10, _config.presition());
	double res = std::round(data*mult) / mult;
	std::string data_to_write = std::to_string(res);
	size_t dot_pos = data_to_write.find(".");
	if (dot_pos != std::string::npos) {
		data_to_write.substr(0, dot_pos + _config.presition() + 1);
		while (!data_to_write.empty() && data_to_write.back() == '0'){
			data_to_write.pop_back();
		}
		if (!data_to_write.empty() && data_to_write.back() == '.') {
			data_to_write.pop_back();
		}
	}
	writer->write_data(data_to_write.c_str());
}

void json::json_writer::write_string(const std::string & data) {
	string_coder coder;
	char quote = _config.has_flag(write_flags::single_quotes) ? '\'' : '\"';
	writer->write_data(std::string(1, quote).c_str());
	writer->write_data(coder.encode(data).c_str());
	writer->write_data(std::string(1, quote).c_str());

}

void json::json_writer::write_array(const json_array & data) {
	bool first = true;
	writer->write_data("[");
	indent_level++;
	for (size_t i = 0; i < data.size(); ++i) {
		first ? first = !first : writer->write_data(",");
		write_indent();
		write(data[i]);
	}
	if (_config.has_flag(write_flags::trailing_commas)) {
		writer->write_data(",");
	}
	indent_level--;
	write_indent();
	writer->write_data("]");
}

void json::json_writer::write_object(const json_object & data) {
	bool first = true;
	writer->write_data("{ ");
	indent_level++;
	for (auto it = data.begin(); it != data.end(); ++it) {
		first ? first = !first : writer->write_data(",");
		write_indent();
		write_string(it->first);
		writer->write_data(":");
		write_space();
		write(it->second);
	}
	if (_config.has_flag(write_flags::trailing_commas)) {
		writer->write_data(",");
	}
	indent_level--;
	write_indent();
	writer->write_data("}");
}