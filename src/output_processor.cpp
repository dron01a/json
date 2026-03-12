#include "output_processor.h"

using namespace json;
using namespace json::io;
using namespace json::io_base;
using namespace json::encodings;

i_output_processor::i_output_processor(i_encoder_ptr_ref dest, bool format)
	: _encoder(dest),
	_indent_level(0),
	_format(format),
	_need_comma(false),
	_after_key(false)
{}

void i_output_processor::write_indent(){
	if (_format) {
		write_new_line();
		for (size_t i = 0; i < _indent_size * _indent_level; ++i) {
			_encoder->encode_code(_indent_char);
		}
	}
}

void i_output_processor::write_space(){
	if (_format) {
		for (size_t i = 0; i < _space_size; ++i) {
			_encoder->encode_code(_space_char);
		}
	}
}

void json::io::i_output_processor::write_new_line() {
	if (_format) {
		_encoder->encode_code('\n');
	}
}

void i_output_processor::set_indent(char _char){
	_indent_char = _char;
}

void i_output_processor::set_space(char _char){
	_space_char = _char;
}

void json::io::i_output_processor::set_indent_size(size_t count) {
	_indent_size = count;
}

void json::io::i_output_processor::set_space_size(size_t count) {
	_space_size = count;
}

void i_output_processor::escape_string(const char * data){
	_encoder->encode_code('\"');
	_encoder->encode_string(data);
	_encoder->encode_code('\"');
}

json_output_processor::json_output_processor(i_encoder_ptr_ref dest, bool format)
	: i_output_processor(dest, format) {}

void json_output_processor::write_bom(){
	_encoder->add_bom();
}

void json_output_processor::write_null() {
	before_value();
	write_space();
	_encoder->encode_string("null");
	end_value();
}

void json_output_processor::write_digit(double data) {
	before_value();
	write_space();
	if (std::isnan(data)) {
		_encoder->encode_string("NaN");
	}
	else if (std::isfinite(data)) {
		if (data < 0) {
			_encoder->encode_code('-');
		}
		_encoder->encode_string("Infinity");
	}
	else {
		std::string data_to_write = std::to_string(data);
		if (data_to_write.find('.') != std::string::npos) {
			data_to_write.erase(data_to_write.find_first_not_of('0') + 1, std::string::npos);
			if (data_to_write.back()) {
				data_to_write.back();
			}
		}
		_encoder->encode_string(data_to_write);
	}
	end_value();
}

void json_output_processor::write_bool(bool data) {
	before_value();
	write_space();
	if (data) {
		_encoder->encode_string("true");
		return;
	}
	_encoder->encode_string("false");
	end_value();
}

void json_output_processor::write_string(const char * data) {
	before_value();
	escape_string(data);
	end_value();
}

void json_output_processor::begin_array() {
	before_value();
	_encoder->encode_code('[');
	_indent_level++;
}

void json_output_processor::end_array() {
	_indent_level--;
	write_indent();
	_encoder->encode_code(']');
	end_value();
}

void json_output_processor::begin_object() {
	before_value();
	_encoder->encode_code('{');
	_indent_level++;
	_need_comma = false;
}

void json_output_processor::end_object() {
	_indent_level--;
	write_indent();
	_encoder->encode_code('}');
	end_value();
}

void json_output_processor::write_key(const char * key) {
	before_value();
	escape_string(key);
	write_space();
	write_colon();
	_need_comma = false;
	_after_key = true;
}

void json::io::json_output_processor::before_value() {
	if (_after_key) {
		_after_key = false;
		return;
	}
	if (_need_comma) {
		write_comma();
	//	write_indent();
	}
	write_indent();
}

void json::io::json_output_processor::end_value(){
	_need_comma = true;
}

void json::io::json_output_processor::write_comma() {
	_encoder->encode_code(',');
	if (_format) {
		write_space();
		//write_new_line();
	}
}

void json::io::json_output_processor::write_colon(){
	_encoder->encode_code(':');
	if (_format) {
		write_space();
	}
}
