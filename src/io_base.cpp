#include "io_base.h"

json::io_base::io_error::io_error(error_code code, size_t & line, size_t & col, std::string file_name)
	: base_error(error_category::io_error, line, col, form_message(code, file_name)) {
}

std::string json::io_base::io_error::form_message(error_code code, std::string file_name) {
	std::string _result;
	switch (code) {
	case json::io_base::io_error::error_code::_file_not_found:
		_result += "file " + file_name + " not found";
		break;
	case json::io_base::io_error::error_code::_string_is_empty:
		_result += "string is empty";
		break;
	case json::io_base::io_error::error_code::_stream_is_bad:
		_result += "bad thread";
		break;
	};
	return _result;
}

json::io_base::file_input::file_input(const std::string & file_name) {
	file.open(file_name, std::ios::in | std::ios::binary);
}

json::io_base::file_input::file_input(const char * file_name) {
	file.open(file_name, std::ios::in | std::ios::binary);
}

json::io_base::file_input::~file_input() {
	if (file.is_open()) {
		file.close();
	}
}

std::char_traits<char>::char_type json::io_base::file_input::next_char() {
	cur_char = file.get();
	if (file.eof()) {
		cur_char = std::char_traits<char>::eof();
	}
	return cur_char;
}

std::char_traits<char>::char_type json::io_base::file_input::last_char() {
	return cur_char;
}

void json::io_base::file_input::seek(int n) {
	file.seekg(n, std::ios::cur);
}

bool json::io_base::file_input::ready() {
	return file.good() && file.is_open();
}

bool json::io_base::file_input::eof(){
	return file.eof();
}

json::io_base::string_input::string_input(const std::string & string) {
	str = string;
}

json::io_base::string_input::string_input(const char * string) {
	str = string;
}

json::io_base::string_input::~string_input(){
}

std::char_traits<char>::char_type json::io_base::string_input::next_char() {
	if (position < str.size()) {
		return str[position++];
	}
	return std::char_traits<char>::eof();
}

std::char_traits<char>::char_type json::io_base::string_input::last_char() {
	if (position < str.size() && position > 0) {
		return str[position - 1];
	}
	if (position == 0) {
		return str[0];
	}
	return str[str.size() - 1];
}

void json::io_base::string_input::seek(int n) {
	if (n <  str.size() - 1) {
		position = n;
	}
}

bool json::io_base::string_input::ready() {
	return !str.empty() && position < str.size();
}

bool json::io_base::string_input::eof(){
	return !(position < str.size());
}

json::io_base::stream_input::stream_input(std::istream & stream) {
	this->stream = &stream;
}

json::io_base::stream_input::~stream_input() {
	stream = nullptr;
}

std::char_traits<char>::char_type json::io_base::stream_input::next_char() {
	*stream >> cur_char;
	if (stream->eof()) {
		cur_char = std::char_traits<char>::eof();
	}
	return cur_char;
}

std::char_traits<char>::char_type json::io_base::stream_input::last_char() {
	return cur_char;
}

void json::io_base::stream_input::seek(int n) {
	stream->seekg(n, std::ios::cur);
}

bool json::io_base::stream_input::ready() {
	return stream->good();
}

bool json::io_base::stream_input::eof(){
	return stream->eof();
}

json::io_base::file_output::file_output(const std::string & file_name) {
	_desc.open(file_name, std::ios::out | std::ios::binary);
}

json::io_base::file_output::file_output(const char * file_name) {
	_desc.open(file_name, std::ios::out | std::ios::binary);
}

json::io_base::file_output::~file_output() {
	_desc.close();
}

void json::io_base::file_output::out_data(const char * data) {
	_desc << data;
}

void json::io_base::file_output::out_data(char data){
	_desc << data;
}

bool json::io_base::file_output::ready() {
	return _desc.good();
}

json::io_base::string_output::string_output(std::string & _data) {
	_desc = &_data;
}

void json::io_base::string_output::out_data(const char * data) {
	*_desc += data;
}

void json::io_base::string_output::out_data(char data){
	*_desc += data;
}

bool json::io_base::string_output::ready() {
	return true;
}

json::io_base::stream_output::stream_output(std::ostream & _data) {
	_desc = &_data;
}

json::io_base::stream_output::~stream_output() {
	_desc = nullptr;
}

void json::io_base::stream_output::out_data(const char * data) {
	*_desc << data;
}

void json::io_base::stream_output::out_data(char data) {
	*_desc << data;
}

bool json::io_base::stream_output::ready() {
	return _desc->good();
}