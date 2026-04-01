#include <iostream>
#include <cassert>

#include "input_processor.h"

#include "../tools/tests_framework.h"

using namespace json;
using namespace json::core::io;
using namespace json::core::io::io_base;

TEST_CASE(string_input_next_char_test) {
	std::string _data = "data";
	input _input(std::make_unique<string_input_policy>(_data));
	for (auto _d : _data) {
		int _c = _input.next_char();
		TEST_ASSERT(_c == _d);
	}
}

TEST_CASE(string_input_last_char_test) {
	std::string _data = "data";
	input _input(std::make_unique<string_input_policy>(_data));
	for (auto _d : _data) {
		_input.next_char();
		TEST_ASSERT(_input.last_char() == _d);
	}
}

TEST_CASE(string_input_seekg_test) {
	std::string _data = "data";
	input _input(std::make_unique<string_input_policy>(_data));
	_input.seekg(2);
	auto _c = _input.next_char();
	TEST_ASSERT(_c == 't');
	_input.seekg(-3);
	_c = _input.next_char();
	TEST_ASSERT(_c == 'd');
}

TEST_CASE(file_input_next_char_test) {
	std::string _file_name = "res/input_test_data.json";
	input _input(std::make_unique<file_input_policy>(_file_name));
	auto _c = _input.next_char();
	while (_c != eof_char()) {
		TEST_ASSERT(_c == _input.last_char());
		_c = _input.next_char();
	}
}

TEST_CASE(file_input_seekg_test) {
	std::string _file_name = "res/input_test_data.json";
	input _input(std::make_unique<file_input_policy>(_file_name));
	_input.seekg(2);
	auto _c = _input.next_char();
	TEST_ASSERT(_c == 't');
	_input.seekg(-3);
	_c = _input.next_char();
	TEST_ASSERT(_c == 'd');
}

TEST_CASE(stream_input_next_char_test) {
	std::string _file_name = "res/input_test_data.json";
	std::ifstream _stream(_file_name, std::ios::in | std::ios::binary);
	input _input(std::make_unique<stream_input_policy>(_stream));
	auto _c = _input.next_char();
	while (_c != eof_char()) {
		TEST_ASSERT(_c == _input.last_char());
		_c = _input.next_char();
	}
	_stream.close();
}

TEST_CASE(stream_input_seekg_test) {
	std::string _file_name = "res/input_test_data.json";
	std::ifstream _stream(_file_name, std::ios::in | std::ios::binary);
	input _input(std::make_unique<stream_input_policy>(_stream));
	_input.seekg(2);
	auto _c = _input.next_char();
	TEST_ASSERT(_c == 't');
	_input.seekg(-3);
	_c = _input.next_char();
	TEST_ASSERT(_c == 'd');
	_stream.close();
}