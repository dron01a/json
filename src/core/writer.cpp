#include "core\writer.h"

using namespace json;
using namespace json::core;
using namespace json::core::io;
using namespace json::core::io::encodings;
using namespace json::core::io::io_base;
using namespace json::core::impl;

write_config::write_config(json::encoding enc, output_format sm, error_mode em) :
	_encoding(enc), _sinax_mode(sm), _error_mode(em), _flags(0) {}

encoding & json::write_config::encoding(){
	return _encoding;
}

output_format & write_config::sinax() {
	return _sinax_mode;
}

write_config::error_mode & write_config::error_halding() {
	return _error_mode;
}

void json::write_config::set_flag(write_flags flag) {
	this->_flags |= (uint32_t)flag;
}

void json::write_config::reset_flag(write_flags flag) {
	_flags ^= (uint32_t)flag & (_flags & (uint32_t)flag);
}

bool json::write_config::chesk_flag(write_flags flag){
	return (_flags & (uint32_t)flag) != 0;
}

char & json::write_config::indent(){
	return _indent_char;
}

char & json::write_config::space(){
	return _space_char;
}

size_t & json::write_config::indent_size() {
	return _indent_size;
}

size_t & json::write_config::space_size(){
	return _space_size;
}

writer_impl::writer_impl(i_output_ptr_ref output, write_config conf) : _output(output) {
	_encoder = make_encoder(conf.encoding(), _output);
	_output_proc = make_output_processor(conf.sinax(), _encoder, conf.chesk_flag(write_flags::format));
	_output_proc->set_indent(conf.indent());
	_output_proc->set_space(conf.chesk_flag(write_flags::using_tabs) ? '\t' : conf.space());
	_output_proc->set_indent_size(conf.indent_size());
	_output_proc->set_space_size(conf.space_size());
}

write_result writer_impl::write(const json_value & json_val){
	write_result res; 
	write_value(json_val, res);
	if (res.errors.size() == 0) {
		res.success = true;
	}
	return res;
}

void writer_impl::write_value(const json_value & json_val, write_result & res) {
	switch (json_val.type()) {
	case value_type::_null:
		_output_proc->write_null();
		break;
	case value_type::_bool:
		_output_proc->write_bool(json_val.as_bool());
		break;
	case value_type::_int:
		_output_proc->write_int(json_val.as_int());
		break;
	case value_type::_uint:
		_output_proc->write_uint(json_val.as_uint());
		break;
	case value_type::_double:
		_output_proc->write_double(json_val.as_double());
		break;
	case value_type::_string:
		_output_proc->write_string(json_val.as_string().c_str());
		break;
	case value_type::_array:
		write_array(*json_val.as_array(), res);
		break;
	case value_type::_object:
		write_object(*json_val.as_object(), res);
		break;
	}
}

void writer_impl::write_object(const json_object & data, write_result & res) {
	_output_proc->begin_object();
	for (auto it = data.begin(); it != data.end(); ++it) {
		_output_proc->write_key(it->first.c_str());
		write_value(it->second, res);
	}
	_output_proc->end_object();
}

void writer_impl::write_array(const json_array & data, write_result & res) {
	_output_proc->begin_array();
	for (auto it = data.begin(); it != data.end(); ++it) {
		write_value(*it, res);
	}
	_output_proc->end_array();
}

write_config json::format() {
	write_config conf;
	conf.set_flag(write_flags::format);
	return conf;
}
