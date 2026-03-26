#include <iostream>
#include <cassert>

#include "input_processor.h"

#include "../tools/tests_framework.h"
#include "../tools/mock_tools.h"

using namespace json;
using namespace json::core::io;

TEST_CASE(base_input_processor_parse_string) {
	class test_proc : public base_input_processor {
	public:
		token next_token(encodings::i_decoder_ptr_ref _decoder) { return token(); };
		using base_input_processor::parse_string;
	};
	test_proc _tp;
	{
		auto _decoder = make_decoder("string with data\"");
		token _result = _tp.parse_string(_decoder, '\"');
		ASSERT_TOKEN_STRING(_result, "string with data");
	}
	{
		auto _decoder = make_decoder("string with\\n data and escape\\t\"");
		token _result = _tp.parse_string(_decoder, '\"');
		ASSERT_TOKEN_STRING(_result, "string with\n data and escape\t");
	}
	{
		auto _decoder = make_decoder("string with unicode data = \\u0041\\u0042\\u0043\" ");
		token _result = _tp.parse_string(_decoder, '\"');
		ASSERT_TOKEN_STRING(_result, "string with unicode data = ABC");
	}
	{
		auto _decoder = make_decoder("\"");
		token _result = _tp.parse_string(_decoder, '\"');
		ASSERT_TOKEN_STRING(_result, "");
	}
}

TEST_CASE(base_input_processor_parse_escape) {
	class test_proc : public base_input_processor {
	public:
		token next_token(encodings::i_decoder_ptr_ref _decoder) { return token(); };
		using base_input_processor::parse_escape;
	};
	test_proc _tp;
	{
		auto _decoder = make_decoder("n");
		token _result = _tp.parse_escape(_decoder);
		ASSERT_TOKEN_STRING(_result, "\n");
	}
	{
		auto _decoder = make_decoder("t");
		token _result = _tp.parse_escape(_decoder);
		ASSERT_TOKEN_STRING(_result, "\t");
	}
	{
		auto _decoder = make_decoder("\\");
		token _result = _tp.parse_escape(_decoder);
		ASSERT_TOKEN_STRING(_result, "\\");
	}
	{
		auto _decoder = make_decoder("u0041");
		token _result = _tp.parse_escape(_decoder);
		ASSERT_TOKEN_STRING(_result, "A");
	}
	{
		auto _decoder = make_decoder("x");
		TEST_ASSERT_THROW(_tp.parse_escape(_decoder), input_error);
	}
}

TEST_CASE(base_input_processor_parse_unicode) {
	class test_proc : public base_input_processor {
	public:
		token next_token(encodings::i_decoder_ptr_ref _decoder) { return token(); };
		using base_input_processor::parse_unicode;
	};
	test_proc _tp;
	{
		auto _decoder = make_decoder("0041");
		token _result = _tp.parse_unicode(_decoder);
		ASSERT_TOKEN_STRING(_result, "A");
	}
	{
		auto _decoder = make_decoder("041F");
		token _result = _tp.parse_unicode(_decoder);
		TEST_ASSERT(_result.string_data().size() == 2);
		TEST_ASSERT((uint8_t)_result.string_data()[0] == 0xD0);
		TEST_ASSERT((uint8_t)_result.string_data()[1] == 0x9F);
	}
	{
		auto _decoder = make_decoder("D83DDE00");
		token _result = _tp.parse_unicode(_decoder);
		TEST_ASSERT(_result.string_data().size() == 4);
	}
	{
		auto _decoder = make_decoder("00XX");
		TEST_ASSERT_THROW(_tp.parse_unicode(_decoder), input_error);
	}
}

TEST_CASE(base_input_processor_parse_number) {
	class test_proc : public base_input_processor {
	public:
		token next_token(encodings::i_decoder_ptr_ref _decoder) { return token(); };
		using base_input_processor::parse_number;
	};
	test_proc _tp;
	{
		auto _decoder = make_decoder("1358.56 ");
		_decoder->next_char();
		token _result = _tp.parse_number(_decoder);
		ASSERT_TOKEN_NUMBER(_result, 1358.56);
	}
	{
		auto _decoder = make_decoder("-0.56 ");
		_decoder->next_char();
		token _result = _tp.parse_number(_decoder);
		ASSERT_TOKEN_NUMBER(_result, -0.56);
	}
	{
		auto _decoder = make_decoder("-5e-2 ");
		_decoder->next_char();
		token _result = _tp.parse_number(_decoder);
		ASSERT_TOKEN_NUMBER(_result, -0.05);
	}
	{
		auto _decoder = make_decoder("1..2 ");
		_decoder->next_char();
		TEST_ASSERT_THROW(_tp.parse_number(_decoder), input_error);
	}
}

TEST_CASE(base_input_processor_parse_literal) {
	class test_proc : public base_input_processor {
	public:
		token next_token(encodings::i_decoder_ptr_ref _decoder) { return token(); };
		using base_input_processor::parse_literal;
	};
	test_proc _tp;
	{
		auto _decoder = make_decoder("true");
		_decoder->next_char();
		token _result = _tp.parse_literal(_decoder, "true", json::core::io::token_type::_true);
		ASSERT_TOKEN_BOOL(_result, json::core::io::token_type::_true);
	}
	{
		auto _decoder = make_decoder("false");
		_decoder->next_char();
		token _result = _tp.parse_literal(_decoder, "false", json::core::io::token_type::_false);
		ASSERT_TOKEN_BOOL(_result, json::core::io::token_type::_false);
	}
	{
		auto _decoder = make_decoder("null");
		_decoder->next_char();
		token _result = _tp.parse_literal(_decoder, "null", json::core::io::token_type::_null);
		TEST_ASSERT(_result.type() == json::core::io::token_type::_null);
	}
	{
		auto _decoder = make_decoder("falseds");
		_decoder->next_char();
		TEST_ASSERT_THROW(_tp.parse_literal(_decoder, "false", json::core::io::token_type::_false), input_error);
	}
}

#ifndef _ALL_TESTS_RUN

int main() {
	tester::inst().run();
	return 0;
}

#endif // _ALL_TESTS_RUN