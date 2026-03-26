#include "core\parser.h"

using namespace json;
using namespace json::core;
using namespace json::core::io;
using namespace json::core::io::encodings;
using namespace json::core::io::io_base;
using namespace json::core::impl;

parse_config::parse_config(json::encoding enc, json_sinax sm, error_mode em) :
	_encoding(enc), _sinax_mode(sm), _error_mode(em) {}

encoding & parse_config::encoding() {
	return _encoding;
}

json_sinax & parse_config::sinax() {
	return _sinax_mode;
}

parse_config::error_mode & parse_config::error_halding() {
	return _error_mode;
}

parse_config json::standart() {
	return parse_config();
}

parse_error::parse_error(error_code code, size_t line, size_t col, std::string context)
	: error(error_category::parse_error, line, col, form_message(code, context)) {}

std::string parse_error::form_message(error_code code, std::string context) {
	std::string _result;
	switch (code) {
	case parse_error::error_code::_invalid_token:
		_result = "error token";
		break;
	case parse_error::error_code::_invalid_value:
		_result = "invalid value";
		break;
	case parse_error::error_code::_invalid_array_value:
		_result = "invalid array value";
		break;
	case parse_error::error_code::_invalid_object_value:
		_result = "invalid object value";
		break;
	}
	if (!context.empty()) {
		_result += ", " + context;
	}
	return _result;
}

dom_parser_impl::dom_parser_impl(i_input_ptr_ref input, parse_config & conf){
	_decoder = make_decoder(conf.encoding(), input);
	_input_proc = make_input_processor(conf.sinax());
	if (conf.error_halding() == parse_config::error_mode::collect) {
		_collect_mode = true;
	}
}

parse_result dom_parser_impl::parse(){
	parse_result result;
	while (!_ref_stack.empty()) { // на случай если парсер был запущен повторно очищаем стек
		_ref_stack.pop();
	}
	_tokenizer = std::make_unique<tokenizer>(_input_proc, _decoder, result.errors);
	_ref_stack.push(std::ref(result.json_val)); // добавляем ссылку на корень
	try {
		_state = state::_start;
		run(result.errors);
		_ref_stack.pop();
	}
	catch (error & err) {
		result.errors.push_back(std::make_unique<error>(err));
	}
	if (result.errors.size() == 0) {
		result.valid = true;
	}
	return result;
}

size_t dom_parser_impl::token_type_to_index(token_type type) {
	switch (type) {
	case token_type::_open_curly_brt:
		return 0;
	case token_type::_close_curly_brt:
		return 1;
	case token_type::_number:
		return 2;
	case token_type::_true:
		return 3;
	case token_type::_false:
		return 3;
	case token_type::_null:
		return 4;
	case token_type::_string:
		return 5;
	case token_type::_open_square_brt:
		return 6;
	case token_type::_close_square_brt:
		return 7;
	case token_type::_comma:
		return 8;
	case token_type::_colon:
		return 9;
	case token_type::_end:
		return 10;
	}
	return 11;
}

parse_error dom_parser_impl::state_to_error(state st){
	switch (st) {
	case dom_parser_impl::state::_start:
		return { parse_error::error_code::_invalid_token,
				 _input_proc->line(),
				 _input_proc->col(),
				 "expected the begin of object, array or value" };
	case dom_parser_impl::state::_obj_begin:
		return { parse_error::error_code::_invalid_token,
				 _input_proc->line(),
				 _input_proc->col(),
				 "expected the key" };
	case dom_parser_impl::state::_key:
		return { parse_error::error_code::_invalid_token,
			     _input_proc->line(),
			     _input_proc->col(), "expected the \":\"" };
	case dom_parser_impl::state::_colon:
		return { parse_error::error_code::_invalid_token,
				 _input_proc->line(),
				 _input_proc->col(),
				 "expected value" };
	case dom_parser_impl::state::_obj_value:
		return { parse_error::error_code::_invalid_object_value,
				 _input_proc->line(),
				 _input_proc->col(),
				 "expected \"}\" or \",\"" };
	case dom_parser_impl::state::_obj_next:
		return { parse_error::error_code::_invalid_object_value,
			     _input_proc->line(),
			     _input_proc->col(),
			     "expected the key" };
	case dom_parser_impl::state::_arr_begin:
		return { parse_error::error_code::_invalid_array_value,
				 _input_proc->line(),
				 _input_proc->col(),
				 "expected the begin of object, array or value" };
	case dom_parser_impl::state::_arr_value:
		return { parse_error::error_code::_invalid_array_value,
				 _input_proc->line(),
				 _input_proc->col(),
				 "expected \"]\" or \",\"" };
	case dom_parser_impl::state::_arr_next:
		return { parse_error::error_code::_invalid_array_value,
				 _input_proc->line(),
				 _input_proc->col(),
				 "expected the begin of object, array or value" };
	case dom_parser_impl::state::_val:
		return { parse_error::error_code::_invalid_value,
			     _input_proc->line(),
			     _input_proc->col(),
			     "expected the end" };
	case dom_parser_impl::state::_obj_end:
		return { parse_error::error_code::_invalid_object_value,
				 _input_proc->line(),
				 _input_proc->col(),
				 "expected \"}\", \"]\" or \",\"" };
	case dom_parser_impl::state::_arr_end:
		return { parse_error::error_code::_invalid_array_value,
				 _input_proc->line(),
				 _input_proc->col(),
				 "expected \"}\", \"]\" or \",\"" };
	}
}

dom_parser_impl::state dom_parser_impl::key_state_handler() {
	_ref_stack.top().get().as_object()->operator[](_tokenizer->last().string_data());
	_ref_stack.push(std::ref(_ref_stack.top().get().as_object()->operator[](_tokenizer->last().string_data())));
	return default_handler();
}

dom_parser_impl::state dom_parser_impl::comma_state_handler() {
	switch (_ref_stack.top().get().type()) {
	case value_type::_array:
		return state::_arr_next;
	case value_type::_object:
		return state::_obj_next;
	}
}

dom_parser_impl::state dom_parser_impl::obj_begin_state_handler() {
	_ref_stack.top().get().type(value_type::_object);
	return default_handler();
}

dom_parser_impl::state dom_parser_impl::obj_value_state_handler() {
	value_handler();
	_ref_stack.pop(); // выбрасываем предыдущий элемент
	return default_handler();
}

dom_parser_impl::state dom_parser_impl::arr_begin_state_handler() {
	_ref_stack.top().get().type(value_type::_array);
	return default_handler();
}

dom_parser_impl::state dom_parser_impl::arr_value_state_handler() {
	_ref_stack.top().get().as_array()->push_back(json_value());
	_ref_stack.push(std::ref(_ref_stack.top().get().as_array()->back()));
	switch (_tokenizer->last().type()) {
	case token_type::_open_curly_brt:
		_perv_state = _state;
		return state::_obj_begin;
	case token_type::_open_square_brt:
		_perv_state = _state;
		return state::_arr_begin;
	}
	value_handler();
	_ref_stack.pop(); // выбрасываем предыдущий элемент
	return default_handler();
}

dom_parser_impl::state dom_parser_impl::value_state_handler() {
	value_handler();
	return default_handler();
}

dom_parser_impl::state dom_parser_impl::default_handler(){
	_tokenizer->next();
	_perv_state = _state;
	return table[(size_t)_state][token_type_to_index(_tokenizer->last().type())];
}

dom_parser_impl::state dom_parser_impl::end_state_handler() {
	if (_ref_stack.size() > 1) {
		_ref_stack.pop();
		return default_handler();
	}
	return state::_end;
}

void dom_parser_impl::value_handler() {
	switch (_tokenizer->last().type()) {
	case token_type::_false:
		_ref_stack.top().get().assign(false);
		break;
	case token_type::_true:
		_ref_stack.top().get().assign(true);
		break;
	case token_type::_string:
		_ref_stack.top().get().assign(_tokenizer->last().string_data());
		break;
	case token_type::_number:
		_ref_stack.top().get().assign(_tokenizer->last().double_data());
		break;
	case token_type::_null:
		_ref_stack.top().get().type(value_type::_null);
		break;
	}
}

dom_parser_impl::state dom_parser_impl::error_handler(err_vect & errors) {
	parse_error _err = state_to_error(_perv_state);
	if (!_collect_mode) {
		throw _err;
	}
	errors.push_back(std::make_unique<parse_error>(_err));
	_state = state::_start;
	return default_handler();
}

void dom_parser_impl::run(err_vect & errors) {
	while (_state != state::_end) {
		switch (_state) {
		case state::_start:
			_state = default_handler();
			break;
		case state::_comma:
			_state = comma_state_handler();
			break;
		case dom_parser_impl::state::_obj_begin:
			_state = obj_begin_state_handler();
			break;
		case dom_parser_impl::state::_key:
			_state = key_state_handler();
			break;
		case dom_parser_impl::state::_obj_value:
			_state = obj_value_state_handler();
			break;
		case dom_parser_impl::state::_arr_begin:
			_state = arr_begin_state_handler();
			break;
		case dom_parser_impl::state::_arr_value:
			_state = arr_value_state_handler();
			break;
		case dom_parser_impl::state::_val:
			_state = value_state_handler();
			break;
		case dom_parser_impl::state::_obj_end:
		case dom_parser_impl::state::_arr_end:
			_state = end_state_handler();
			break;
		case dom_parser_impl::state::_err:
			_state = error_handler(errors);
			break;
		default:
			_state = default_handler();
			break;
		}
	}
}