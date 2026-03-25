#include "io_base.h"

using namespace json;
using namespace json::io_base;

io_error::io_error(error_code code, size_t line, size_t col, std::string file_name)
	: base_error(error_category::io_error, line, col, form_message(code, file_name)) {
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

file_input::file_input(const std::string & file_name) {
	file.open(file_name, std::ios::in | std::ios::binary);
}

file_input::file_input(const char * file_name) {
	file.open(file_name, std::ios::in | std::ios::binary);
}

file_input::~file_input() {
	if (file.is_open()) {
		file.close();
	}
}

int file_input::next_char() {
	cur_char = file.get();
	if (file.eof()) {
		cur_char = std::char_traits<char>::eof();
	}
	return cur_char;
}

int file_input::last_char() {
	return cur_char;
}

void file_input::seek(int n) {
	file.seekg(n, std::ios::cur);
}

bool file_input::ready() {
	return file.good() && file.is_open();
}

bool file_input::eof(){
	return file.eof();
}

string_input::string_input(const std::string & string) {
	str = string;
}

string_input::string_input(const char * string) {
	str = string;
}

string_input::~string_input(){}

int string_input::next_char() {
	if (position < str.size() && position != -1) {
		return str[position++];
	}
	position = -1;
	return std::char_traits<char>::eof();
}

int string_input::last_char() {
	if (position != -1) {
		return str[position];
	}
	if (position == 0) {
		return str[0];
	}
	return std::char_traits<char>::eof();
}

void string_input::seek(int n) {
	if (position +  n <  str.size() - 1 && position + n >= 0) {
		position += n;
	}
}

bool string_input::ready() {
	return !str.empty() && position < str.size() && position >= 0;
}

bool string_input::eof(){
	return !(position < str.size() - 1);
}

stream_input::stream_input(std::istream & stream) {
	this->stream = &stream;
}

stream_input::~stream_input() {
	stream = nullptr;
}

int stream_input::next_char() {
	*stream >> cur_char;
	if (stream->eof()) {
		cur_char = std::char_traits<char>::eof();
	}
	return cur_char;
}

int stream_input::last_char() {
	return cur_char;
}

void stream_input::seek(int n) {
	stream->seekg(n, std::ios::cur);
}

bool stream_input::ready() {
	return stream->good();
}

bool stream_input::eof(){
	return stream->eof();
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

void json::io_base::file_output::out_data(char32_t data){
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

void json::io_base::string_output::out_data(char32_t data){
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

void json::io_base::stream_output::out_data(char32_t data){
	*_desc << data;
}

bool stream_output::ready() {
	return _desc->good();
}