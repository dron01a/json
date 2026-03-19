#include "parser.h"

using namespace json;
using namespace json::io;
using namespace json::io_base;
using namespace json::impl;
using namespace json::encodings;

parse_config::parse_config(encoding_mode enc, sinax_mode sm, error_mode em) :
	_encoding(enc), _sinax_mode(sm), _error_mode(em) {}

parse_config::encoding_mode & parse_config::encoding() {
	return _encoding;
}

parse_config::sinax_mode & parse_config::sinax() {
	return _sinax_mode;
}

parse_config::error_mode & parse_config::error_halding() {
	return _error_mode;
}

parse_config json::standart() {
	return parse_config();
}

parse_error::parse_error(error_code code, size_t line, size_t col)
	: base_error(error_category::parse_error, line, col, form_message(code)) {}

std::string parse_error::form_message(error_code code) {
	switch (code) {
	case parse_error::error_code::_error_token:
		return "error token";
	case parse_error::error_code::_invalid_value:
		return "invalid value";
	case parse_error::error_code::_invalid_array_value:
		return "invalid array value";
	case parse_error::error_code::_invalid_object:
		return "invalid object";
	}
}

dom_parser_impl::dom_parser_impl(i_input_ptr_ref input, parse_config & conf){
	switch (conf.encoding()) {
	case parse_config::encoding_mode::ascii:
		_decoder = std::make_unique<ascii_decoder>(input);
		break;
	case parse_config::encoding_mode::utf8:
		_decoder = std::make_unique<utf8_decoder>(input);
		break;
	}
	switch (conf.sinax()) {
	case parse_config::sinax_mode::STANDART:
		_input_proc = std::make_unique<json_input_processor>();
		break;
	case parse_config::sinax_mode::JSON5:
		_input_proc = std::make_unique<json5_input_processor>();
		break;
	/*case parse_config::sinax_mode::YALM:
		_input_proc = std::make_unique<yalm_input_processor>();
		break;*/
	}
	if (conf.error_halding() == parse_config::error_mode::collect) {
		_collect_mode = true;
	}
}

parse_result dom_parser_impl::parse(){
	parse_result result;
	_tokenizer = std::make_unique<tokenizer>(_input_proc, _decoder, result.errors);
	_ref_stack.push(std::ref(result.json_val)); // добавляем ссылку на корень
	try {
		run(result.errors);
		_ref_stack.pop();
	}
	catch (base_error & err) {
		result.errors.push_back(std::make_unique<base_error>(err));
	}
	if (result.errors.size() == 0) {
		result.valid = true;
	}
	return result;
}

void dom_parser_impl::error_handler(token & cur, err_vect & errors, parse_error::error_code code) {
	if (!_collect_mode) {
		throw parse_error(code, _input_proc->line(), _input_proc->col());
	}
	errors.push_back(std::make_unique<parse_error>(code, _input_proc->line(), _input_proc->col()));
	_state = _perv_state;
}

size_t json::impl::dom_parser_impl::token_type_to_index(token_type type) {
	switch (type) {
	case json::io::token_type::_open_curly_brt:
		return 0;
	case json::io::token_type::_close_curly_brt:
		return 1;
	case json::io::token_type::_number:
		return 2;
	case json::io::token_type::_true:
		return 3;
	case json::io::token_type::_false:
		return 3;
	case json::io::token_type::_null:
		return 4;
	case json::io::token_type::_string:
		return 5;
	case json::io::token_type::_open_square_brt:
		return 6;
	case json::io::token_type::_close_square_brt:
		return 7;
	case json::io::token_type::_comma:
		return 8;
	case json::io::token_type::_colon:
		return 9;
	case json::io::token_type::_end:
		return 10;
	}
	return 11;
}

void json::impl::dom_parser_impl::key_state_handler(token & cur) {
	_ref_stack.top().get().as_object()->operator[](cur.string_data());
	_ref_stack.push(std::ref(_ref_stack.top().get().as_object()->operator[](cur.string_data())));
}

void json::impl::dom_parser_impl::value_state_handler(token & cur) {
	switch (cur.type()) {
	case token_type::_false:
		_ref_stack.top().get().assign(false);
		break;
	case token_type::_true:
		_ref_stack.top().get().assign(true);
		break;
	case token_type::_string:
		_ref_stack.top().get().assign(cur.string_data());
		break;
	case token_type::_number:
		_ref_stack.top().get().assign(cur.double_data());
		break;
	case token_type::_null:
		_ref_stack.top().get().type(value_type::_null);
		break;
	default:
		// to-do проверить не нужен ли throw
		break;
	}
	
}

void json::impl::dom_parser_impl::end_state_handler(token & cur) {
	if (_ref_stack.size() > 1) {
	    _ref_stack.pop();
		switch (_ref_stack.top().get().type()) {
		case value_type::_array:
			_state = state::_arr_value;
			break;
		case value_type::_object:
			_state = state::_obj_value;
			break;
		}
	}
	else {
		_state = state::_end;
	}
}

void json::impl::dom_parser_impl::run(err_vect & errors) {
	token cur_token = _tokenizer->next();
	_state = table[(size_t)_state][token_type_to_index(cur_token.type())];
	_perv_state = _state;
	while (_state != state::_end) {
		switch (_state) {
		case json::impl::dom_parser_impl::state::_obj_begin:
			_ref_stack.top().get().type(value_type::_object);
			break;
		case json::impl::dom_parser_impl::state::_key:
			key_state_handler(cur_token);
			break;
		case json::impl::dom_parser_impl::state::_obj_value:
			value_state_handler(cur_token);
			_ref_stack.pop(); // выбрасываем предыдущий элемент
			break;
		case json::impl::dom_parser_impl::state::_arr_begin:
			_ref_stack.top().get().type(value_type::_array);
			break;
		case json::impl::dom_parser_impl::state::_arr_value:
			_ref_stack.top().get().as_array()->push_back(json_value());
			_ref_stack.push(std::ref(_ref_stack.top().get().as_array()->back()));
			switch (cur_token.type()) {
			case token_type::_open_curly_brt:
				_perv_state = _state;
				_state = state::_obj_begin;
				continue;
			case token_type::_open_square_brt:
				_perv_state = _state;
				_state = state::_arr_begin;
				continue;
			}
			value_state_handler(cur_token);
			_ref_stack.pop(); // выбрасываем предыдущий элемент
			break;
		case json::impl::dom_parser_impl::state::_val:
			value_state_handler(cur_token);
			break;
		case json::impl::dom_parser_impl::state::_err:
			error_handler(cur_token, errors, parse_error::error_code::_error_token);
			continue;
		case json::impl::dom_parser_impl::state::_obj_end:
		case json::impl::dom_parser_impl::state::_arr_end:
			end_state_handler(cur_token);
			break;
		}
		cur_token = _tokenizer->next();
		_perv_state = _state;
		_state = table[(size_t)_state][token_type_to_index(cur_token.type())];
	}
}