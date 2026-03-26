#include "core\output_processor.h"

using namespace json;
using namespace json::core::io;
using namespace json::core::io::io_base;
using namespace json::core::io::encodings;

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

void i_output_processor::write_new_line() {
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

void i_output_processor::set_indent_size(size_t count) {
	_indent_size = count;
}

void i_output_processor::set_space_size(size_t count) {
	_space_size = count;
}

encodings::i_encoder_ptr_ref i_output_processor::encoder(){
	return _encoder;
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

void json_output_processor::write_int(int data) {
	before_value();
	write_space();
	if (std::isnan((double)data)) {
		_encoder->encode_string("NaN");
	}
	else if (std::isinf((double)data)) {
		if (data < 0) {
			_encoder->encode_code('-');
		}
		_encoder->encode_string("Infinity");
	}
	else {
		_encoder->encode_string(std::to_string(data));
	}
	end_value();
}

void json_output_processor::write_uint(unsigned int data) {
	before_value();
	write_space();
	if (std::isnan((double)data)) {
		_encoder->encode_string("NaN");
	}
	else if (std::isinf((double)data)) {
		if (data < 0) {
			_encoder->encode_code('-');
		}
		_encoder->encode_string("Infinity");
	}
	else {
		_encoder->encode_string(std::to_string(data));
	}
	end_value();
}

void json_output_processor::write_double(double data) {
	before_value();
	if (std::isnan((double)data)) {
		_encoder->encode_string("NaN");
	}
	else if (std::isinf((double)data)) {
		if (data < 0) {
			_encoder->encode_code('-');
		}
		_encoder->encode_string("Infinity");
	}
	else {
		std::string data_to_write = std::to_string(data);
		if (data_to_write.find('.') != std::string::npos) {
			while (data_to_write.back() == '0') {
				data_to_write.pop_back();
			}
			if (data_to_write.back() == '.') {
				data_to_write.pop_back();
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

void json_output_processor::before_value() {
	if (_after_key) {
		_after_key = false;
		return;
	}
	if (_need_comma) {
		write_comma();
	}
	write_indent();
}

void json_output_processor::end_value(){
	_need_comma = true;
}

void json_output_processor::write_comma() {
	_encoder->encode_code(',');
	if (_format) {
		write_space();
	}
}

void json_output_processor::write_colon(){
	_encoder->encode_code(':');
	if (_format) {
		write_space();
	}
}

xml_output_processor::xml_output_processor(encodings::i_encoder_ptr_ref dest, bool format)
	: i_output_processor(dest, format) , _in_array_flag(false), _arr_level(0) {
	write_key("root");
	_indent_level--;
}

xml_output_processor::xml_output_processor(encodings::i_encoder_ptr_ref dest, std::string & root_name, bool format) 
	: i_output_processor(dest, format), _in_array_flag(false), _arr_level(0) {
	write_key(root_name.c_str());
	_indent_level--;
}

void xml_output_processor::write_bom() {
	_encoder->add_bom();
}

void xml_output_processor::write_null() {
	before_value();
	_encoder->encode_string("0");
	_indent_level--;
	write_close_tag();
}

void xml_output_processor::write_int(int data) {
	before_value();
	if (std::isnan((double)data) || std::isinf((double)data)) {
		_encoder->encode_string("NaN");
	}
	else {
		_encoder->encode_string(std::to_string(data));
	}
	_indent_level--;
	write_close_tag();
}

void xml_output_processor::write_uint(unsigned int data) {
	before_value();
	if (std::isnan((double)data) || std::isinf((double)data)) {
		_encoder->encode_string("NaN");
	}
	else {
		_encoder->encode_string(std::to_string(data));
	}
	_indent_level--;
	write_close_tag();
}

void xml_output_processor::write_double(double data) {
	before_value();
	if (std::isnan((double)data) || std::isinf((double)data)) {
		_encoder->encode_string("NaN");
	}
	else {
		std::string data_to_write = std::to_string(data);
		if (data_to_write.find('.') != std::string::npos) {
			while (data_to_write.back() == '0') {
				data_to_write.pop_back();
			}
			if (data_to_write.back() == '.') {
				data_to_write.pop_back();
			}
		}
		_encoder->encode_string(data_to_write);
	}
	_indent_level--;
	write_close_tag();
}

void xml_output_processor::write_bool(bool data) {
	before_value();
	if (data) {
		_encoder->encode_string("\"true\"");
	}
	else {
		_encoder->encode_string("\"false\"");
	}
	_indent_level--;
	write_close_tag();
}

void xml_output_processor::write_string(const char * data) {
	before_value();
	_encoder->encode_string(data);
	_indent_level--;
	write_close_tag();
}

void xml_output_processor::begin_array() {
	_arr_level++;
	_in_array_flag = true;
}

void xml_output_processor::end_array() {
	if (_arr_level == 1) {
		_in_array_flag = false;
	}
	_indent_level--;
	_arr_level--;
	write_close_tag();
}

void xml_output_processor::begin_object() {
	if (_obj_level == 0 ) {
		_indent_level++;
	}
	_obj_level++;
}

void xml_output_processor::end_object() {
	_indent_level--;
	_obj_level--;
	write_close_tag();
}

void xml_output_processor::write_key(const char * key) {
	write_indent();
	_tag_names.push(key);
	_encoder->encode_string("<" + _tag_names.top() + ">");
	_indent_level++;
}

void xml_output_processor::before_value() {
	if (_in_array_flag) {
		write_key("array_item");
	}
	write_space();
	write_indent();
}

void xml_output_processor::write_close_tag() {
	if (!_tag_names.empty()) {
		write_indent();
		_encoder->encode_code('<');
		_encoder->encode_code('\\');
		_encoder->encode_string(_tag_names.top() + ">");
		_tag_names.pop();
	}
}
