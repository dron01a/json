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

parse_config standart() {
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
	_tokenizer->next();
	try {
		result.json_val = parse_json_value(result.errors);
	}
	catch (base_error & err) {
		result.errors.push_back(std::make_unique<base_error>(err));
	}
	if (result.errors.size() == 0) {
		result.valid = true;
	}
	return result;
}

void dom_parser_impl::set_collect_mode(bool val){
	_collect_mode = val;
}

json_value dom_parser_impl::parse_json_value(std::vector<std::unique_ptr<base_error>> & errors){
	token cur_token = _tokenizer->last();
	switch (cur_token.type()) {
	case token_type::_open_curly_brt:
		return parse_object(errors);
	case token_type::_open_square_brt:
		return parse_array(errors);
	case token_type::_null:
		return json_value();
	case token_type::_number:
		return json_value(cur_token.double_data());
	case token_type::_string:
		return json_value(cur_token.string_data().c_str());
	case token_type::_true:
		return json_value(true);
	case token_type::_false:
		return json_value(false);
	case token_type::_end:
		return json_value();
	}
	collect_or_throw(errors, parse_error::error_code::_invalid_value);
}

json_value dom_parser_impl::parse_array(std::vector<std::unique_ptr<base_error>> & errors){
	json_value result(value_type::_array);
	bool _parse = true;
	_tokenizer->next();
	while (_parse && _tokenizer->last().type() != token_type::_close_square_brt) {
		result.as_array()->push_back(parse_json_value(errors));
		_tokenizer->next();
		switch (_tokenizer->last().type()) {
		case token_type::_comma:
			_tokenizer->next();
			break;
		case token_type::_close_square_brt:
			_parse = false;
			break;
		default:
			collect_or_throw(errors, parse_error::error_code::_invalid_array_value);
		}
	}
	if (!_parse && _tokenizer->last().type() != token_type::_close_square_brt) {
		collect_or_throw(errors, parse_error::error_code::_invalid_array_value);
	}
	return result;
}

json_value dom_parser_impl::parse_object(std::vector<std::unique_ptr<base_error>> & errors){
	json_value result(value_type::_object);
	_tokenizer->next();
	while (_tokenizer->last().type() != token_type::_close_curly_brt) {
		if (_tokenizer->last().type() == token_type::_string) {
			std::string key = _tokenizer->last().string_data();
			_tokenizer->next();
			if (_tokenizer->last().type() != token_type::_colon) {
				collect_or_throw(errors, parse_error::error_code::_invalid_object);
			}
			_tokenizer->next();
			json_value val = parse_json_value(errors);
			result.as_object()->insert({ std::move(key), std::move(val) });
			_tokenizer->next();
			continue;
		}
		if (_tokenizer->last().type() == token_type::_comma) {
			_tokenizer->next();
			continue;
		}
		collect_or_throw(errors, parse_error::error_code::_error_token);
	}
	return result;
}

void dom_parser_impl::collect_or_throw(std::vector<std::unique_ptr<base_error>>& errors, parse_error::error_code code){
	if (!_collect_mode) {
		throw parse_error(code, _input_proc->line(), _input_proc->col());
	}
	errors.push_back(std::make_unique<parse_error>(code, _input_proc->line(), _input_proc->col()));
	_tokenizer->next();
}
