#include "io_base.h"

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

char json::io_base::file_input::next_char() {
	cur_char = file.get();
	if (file.eof()) {
		cur_char = '\0';
	}
	return cur_char;
}

char & json::io_base::file_input::last_char() {
	return cur_char;
}

void json::io_base::file_input::step_back(int n) {
	file.seekg(-n, std::ios::cur);
}

bool json::io_base::file_input::ready() {
	return file.good() && file.is_open();
}

json::io_base::string_input::string_input(const std::string & string) {
	str = string;
}

json::io_base::string_input::string_input(const char * string) {
	str = string;
}

char json::io_base::string_input::next_char() {
	if (position < str.size()) {
		return str[position++];
	}
	return '\0';
}

char & json::io_base::string_input::last_char() {
	if (position < str.size() && position > 0) {
		return str[position - 1];
	}
	if (position == 0) {
		return str[0];
	}
	return str[str.size() - 1];
}

void json::io_base::string_input::step_back(int n) {
	if (position - n >= 0) {
		position -= n;
	}
}

bool json::io_base::string_input::ready() {
	return !str.empty() && position < str.size();
}

json::io_base::stream_input::stream_input(std::istream & stream) {
	this->stream = &stream;
}

json::io_base::stream_input::~stream_input() {
	stream = nullptr;
}

char json::io_base::stream_input::next_char() {
	*stream >> cur_char;
	if (stream->eof()) {
		cur_char = '\0';
	}
	return cur_char;
}

char & json::io_base::stream_input::last_char() {
	return cur_char;
}

void json::io_base::stream_input::step_back(int n) {
	stream->seekg(-n, std::ios::cur);
}

bool json::io_base::stream_input::ready() {
	return stream->good();
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

bool json::io_base::file_output::ready() {
	return _desc.good();
}

json::io_base::string_output::string_output(std::string & _data) {
	_desc = &_data;
}

void json::io_base::string_output::out_data(const char * data) {
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

bool json::io_base::stream_output::ready() {
	return _desc->good();
}