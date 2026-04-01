#include <iostream>
#include <cassert>

#include "encoding.h"

#include "../tools/tests_framework.h"
#include "../tools/mock_tools.h"

using namespace json;
using namespace json::core::io;
using namespace json::core::io::encodings;
using namespace json::core::io::io_base;

TEST_CASE(utf8_decoder_next_char_test) {
	input _input(std::make_unique<string_input_policy>("data€€"));
	utf8_decoder _decoder(_input);
	std::string _read_res;
	char32_t _c = _decoder.next_char();
	while (_c != eof_char()) {
		_read_res += static_cast<char>(_c);
		_c = _decoder.next_char();
	}
	TEST_ASSERT(_read_res[0] == 'd');
	TEST_ASSERT(_read_res[1] == 'a');
	TEST_ASSERT(_read_res[2] == 't');
	TEST_ASSERT(_read_res[3] == 'a');
	TEST_ASSERT(_read_res[4] == '€');
}

TEST_CASE(utf8_decoder_current_char_test) {
	input _input(std::make_unique<string_input_policy>("data€€"));
	utf8_decoder _decoder(_input);
	char32_t _c = _decoder.next_char();
	while (_c != eof_char()) {
		_c = _decoder.next_char();
		TEST_ASSERT(_c == _decoder.current_char());
	}
}

TEST_CASE(utf8_decoder_peek_char_test) {
	input _input(std::make_unique<string_input_policy>("data€€"));
	utf8_decoder _decoder(_input);
	char32_t _c = _decoder.next_char();
	char32_t _peek = _decoder.peek_char();
	TEST_ASSERT(_c == 'd');
	TEST_ASSERT(_peek == 'a');
	_c = _decoder.next_char();
	TEST_ASSERT(_c == 'a');
}

TEST_CASE(utf8_decoder_push_buff_test) {
	input _input(std::make_unique<string_input_policy>("data€€"));
	utf8_decoder _decoder(_input);
	_decoder.push_buff('c');
	_decoder.push_buff('b');
	TEST_ASSERT(_decoder.next_char() == 'b');
	TEST_ASSERT(_decoder.next_char() == 'c');
	TEST_ASSERT(_decoder.next_char() == 'd');
}