#include "output_processor.h"

using namespace json;
using namespace json::io;
using namespace json::io_base;
using namespace json::encodings;

i_output_processor::i_output_processor(i_encoder_ptr_ref dest,
									   bool format,
									   size_t indent_size, 
									   size_t space_size) 
	: _encoder(dest),
	_indent_size(indent_size),
	_space_size(space_size), 
	_space_char(' '), 
	_indent_char(' '),
	_indent_level(0),
	_format(format)
{}

void i_output_processor::write_indent(){
	if (_format) {
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

void i_output_processor::escape_string(const char * data){
	_encoder->encode_code('\"');
	while (*data != '\0') {
		switch (*data){
		case '\"':
			_encoder->encode_string("\\\"");
			break;
		case '\\':
			_encoder->encode_string("\\\\");
			break;
		case '/':
			_encoder->encode_string("\\/");
			break;
		case '\b':
			_encoder->encode_string("\\b");
			break;
		case '\f':
			_encoder->encode_string("\\f");
			break;
		case '\n':
			_encoder->encode_string("\\n");
			break;
		case '\r':
			_encoder->encode_string("\\r");
			break;
		case '\t':
			_encoder->encode_string("\\t");
			break;
		default:
			if (*data < 0x20) {
				char buf[7];
				snprintf(buf, sizeof(buf), "\\u%04X", *data);
				_encoder->encode_string(buf);
			}
			else {
				_encoder->encode_code(*data);
			}
			break;
		}
		data++;
	}
	_encoder->encode_code('\"');
}

json_output_processor::json_output_processor(i_encoder_ptr_ref dest,
										     bool format,
											 size_t indent_size, 
											 size_t space_size)
	: i_output_processor(dest, format, indent_size, space_size) {}

void json_output_processor::write_bom(){
	_encoder->add_bom();
}

void json_output_processor::write_null() {
	write_space();
	_encoder->encode_string("null");
}

void json_output_processor::write_digit(double data) {
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
}

void json_output_processor::write_bool(bool data) {
	write_space();
	if (data) {
		_encoder->encode_string("true");
		return;
	}
	_encoder->encode_string("false");
}

void json_output_processor::write_string(const char * data) {
	escape_string(data);
}

void json_output_processor::begin_array() {
	_encoder->encode_code('[');
	_indent_level++;
}

void json_output_processor::end_array() {
	_indent_level--;
	_encoder->encode_code(']');
}

void json_output_processor::begin_object() {
	_encoder->encode_code('{');
	_indent_level++;
}

void json_output_processor::end_object() {
	_indent_level--;
	_encoder->encode_code('}');
}

void json_output_processor::write_key(const char * key) {
	escape_string(key);
	write_space();
	write_colon();
}

void json::io::json_output_processor::write_comma() {
	_encoder->encode_code(',');
	if (_format) {
		write_space();
	}
}

void json::io::json_output_processor::write_colon(){
	_encoder->encode_code(',');
	if (_format) {
		write_space();
	}
}
