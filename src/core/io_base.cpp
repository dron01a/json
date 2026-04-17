#include "core\io_base.h"

using namespace json;
using namespace json::core::io;
using namespace json::core::io::io_base;

io_error::io_error(error_code code, size_t line, size_t col, std::string file_name)
	: error(error_category::io_error, line, col, form_message(code, file_name)) {
}

std::string io_error::form_message(error_code code, std::string file_name) {
	switch (code) {
	case io_error::error_code::_file_not_found:
		return "file " + file_name + " not found";
	case io_error::error_code::_string_is_empty:
		return "string is empty";
	case io_error::error_code::_stream_is_bad:
		return "bad thread";
	case io_error::error_code::_invalid_file:
		return "invalid file: " + file_name;
	case io_error::error_code::_invalid_string:
		return "invalid string";
	case io_error::error_code::_invalid_stream:
		return "invalid stream";
	};
}

int_type json::core::io::io_base::eof_char() {
	return json::core::io::io_base::char_traits::eof();
}

json::core::io::io_base::string_input_policy::string_input_policy(const std::string & src) {
	_src = src;
	_pos = 0;
}

size_t json::core::io::io_base::string_input_policy::fill_buff(char * buff, size_t buff_size) {
	if (eof()) {
		return 0;
	}
	const char * c_ptr = _src.c_str();
	size_t aval = _src.size() - _pos;
	size_t size_to_copy = (buff_size < aval) ? buff_size : aval;
	memcpy(buff, c_ptr + _pos, size_to_copy);
	_pos += size_to_copy;
	if (size_to_copy < buff_size) {
		buff[size_to_copy] = eof_char();
	}
	return size_to_copy;
}

bool json::core::io::io_base::string_input_policy::eof() {
	return !(_pos <= _src.size() - 1);
}

void json::core::io::io_base::string_input_policy::seekg(int pos, dir dir) {
	switch (dir) {
	case json::core::io::io_base::dir::_begin:
		if (pos < 0) {
			return;
		}
		_pos = pos;
		break;
	case json::core::io::io_base::dir::_cur:
		if (pos < 0 && _pos + pos < 0) {
			return;
		}
		_pos += pos;
		break;
	case json::core::io::io_base::dir::_end:
		if (pos < 0) {
			return;
		}
		_pos = _src.size() - pos;
		break;
	}
}

size_t json::core::io::io_base::string_input_policy::pos() {
	return _pos;
}

bool json::core::io::io_base::string_input_policy::good() {
	return !_src.empty();
}

json::core::io::io_base::file_input_policy::file_input_policy(const std::string & file_name) {
	_file.open(file_name, std::ios::in | std::ios::binary);
	valid = _file.good();
}

json::core::io::io_base::file_input_policy::~file_input_policy() {
	_file.close();
}

size_t json::core::io::io_base::file_input_policy::fill_buff(char * buff, size_t buff_size) {
	if (!_file.eof()) {
		_file.read(buff, buff_size);
		std::streamsize len = _file.gcount();
		if (len < buff_size) {
			buff[len] = eof_char();
		}
		return len;
	}
	valid = false;
	return 0;
}

bool json::core::io::io_base::file_input_policy::eof() {
	return _file.eof();
}

void json::core::io::io_base::file_input_policy::seekg(int pos, dir dir) {
	_file.clear();
	switch (dir) {
	case json::core::io::io_base::dir::_begin:
		_file.seekg(pos, std::ios_base::beg);
		break;
	case json::core::io::io_base::dir::_cur:
		_file.seekg(pos, std::ios_base::cur);
		break;
	case json::core::io::io_base::dir::_end:
		_file.seekg(pos, std::ios_base::end);
		break;
	}
}

size_t json::core::io::io_base::file_input_policy::pos() {
	return (size_t)_file.tellg();
}

bool json::core::io::io_base::file_input_policy::good() {
	return valid;
}

json::core::io::io_base::stream_input_policy::stream_input_policy(std::istream & stream) {
	_stream = &stream;
}

json::core::io::io_base::stream_input_policy::~stream_input_policy() {
	_stream = nullptr;
}

size_t json::core::io::io_base::stream_input_policy::fill_buff(char * buff, size_t buff_size) {
	if (!_stream->eof()) {
		_stream->read(buff, buff_size);
		std::streamsize len = _stream->gcount();
		if (len < buff_size) {
			buff[len] = eof_char();
		}
		return len;
	}
	return 0;
}

bool json::core::io::io_base::stream_input_policy::eof() {
	return _stream->eof();
}

void json::core::io::io_base::stream_input_policy::seekg(int pos, dir dir) {
	_stream->clear();
	switch (dir) {
	case json::core::io::io_base::dir::_begin:
		_stream->seekg(pos, std::ios_base::beg);
		break;
	case json::core::io::io_base::dir::_cur:
		_stream->seekg(pos, std::ios_base::cur);
		break;
	case json::core::io::io_base::dir::_end:
		_stream->seekg(pos, std::ios_base::end);
		break;
	}
}

size_t json::core::io::io_base::stream_input_policy::pos() {
	return _stream->tellg();
}

bool json::core::io::io_base::stream_input_policy::good() {
	return _stream->good();
}

json::core::io::io_base::input::input(std::unique_ptr<input_policy> policy) : _buff_pos(0), _policy(std::move(policy)) {
	_buff_size = _policy->fill_buff(_buff, INPUT_BUFFER_SIZE);
}

int_type json::core::io::io_base::input::next_char() {
	if (_buff_pos >= _buff_size && !_policy->eof()) {
		_buff_pos = 0;
		_buff_size = _policy->fill_buff(_buff, INPUT_BUFFER_SIZE);
		if (_buff_size == 0) {
			return eof_char();
		}
	}
	return _buff[_buff_pos++];
}

int_type json::core::io::io_base::input::last_char() {
	if (_buff_pos == 0) {
		return _buff[0];
	}
	if (_buff_pos <= _buff_size) {
		return _buff[_buff_pos - 1];
	}
	return eof_char();
}

void json::core::io::io_base::input::seekg(int pos, dir dir) {
	if (_buff_pos + pos >= 0 && _buff_pos + pos < _buff_size) {
		_buff_pos += pos;
	}
	else {
		_policy->seekg(pos, dir);
		_buff_size = _policy->fill_buff(_buff, INPUT_BUFFER_SIZE);
		_buff_pos = 0;
	}
}

bool json::core::io::io_base::input::good() {
	return _policy->good();
}

bool json::core::io::io_base::input::eof() {
	return _policy->eof() && _buff[_buff_pos] == eof_char();
}


file_output::file_output(const std::string & file_name) {
	_desc.open(file_name, std::ios::out | std::ios::binary);
}

file_output::file_output(const char * file_name) {
	_desc.open(file_name, std::ios::out | std::ios::binary);
}

file_output::~file_output() {
	_desc.close();
}

void file_output::out_data(const char * data) {
	_desc << data;
}

void file_output::out_data(char data){
	_desc << data;
}

void file_output::out_data(char32_t data){
	_desc << data;
}

bool file_output::ready() {
	return _desc.good();
}

string_output::string_output(std::string & _data) {
	_desc = &_data;
}

void string_output::out_data(const char * data) {
	*_desc += data;
}

void string_output::out_data(char data){
	*_desc += data;
}

void string_output::out_data(char32_t data){
	_desc += data;
}

bool string_output::ready() {
	return true;
}

stream_output::stream_output(std::ostream & _data) {
	_desc = &_data;
}

stream_output::~stream_output() {
	_desc = nullptr;
}

void stream_output::out_data(const char * data) {
	*_desc << data;
}

void stream_output::out_data(char data) {
	*_desc << data;
}

void stream_output::out_data(char32_t data){
	*_desc << data;
}

bool stream_output::ready() {
	return _desc->good();
}