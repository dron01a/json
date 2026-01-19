#include "json_writer.h"

json::string_writer::string_writer(std::string & _data) {
	_desc = &_data;
}

void json::string_writer::write_data(const char * data) {
	*_desc += data;
}

bool json::string_writer::ready() {
	return true;
}

json::file_writer::file_writer(const std::string & file_name) {
	_desc.open(file_name, std::ios::out | std::ios::binary);
}

json::file_writer::file_writer(const char * file_name) {
	_desc.open(file_name, std::ios::out | std::ios::binary);
}

json::file_writer::~file_writer() {
	_desc.close();
}

void json::file_writer::write_data(const char * data) {
	_desc << data;
}

bool json::file_writer::ready() {
	return _desc.good();
}

json::stream_writer::stream_writer(std::ostream & _data) {
	_desc = &_data;
}

json::stream_writer::~stream_writer() {
	_desc = nullptr;
}

void json::stream_writer::write_data(const char * data) {
	*_desc << data;
}

bool json::stream_writer::ready() {
	return _desc->good();
}



json::write_config json::compact() {
	write_config conf;
	conf.reset();

	return conf;
}

json::write_config json::format() {
	write_config conf;
	conf.set_flag(write_flags::format);
	conf.space_count(1);
	conf.indent("\t");
	return conf;
}

json::write_config::write_config() : _flags(0) {}

void json::write_config::set_flag(write_flags flag) {
	this->_flags |= (uint32_t)flag;
}

void json::write_config::reset_flag(write_flags flag) {
	_flags ^= (uint32_t)flag & (_flags & (uint32_t)flag);
}

void json::write_config::reset() {
	_flags = 0;
}

bool json::write_config::has_flag(write_flags flag) {
	return (_flags & (uint32_t)flag) != 0;
}

void json::write_config::indent(const char * str) {
	_indent = str;
}

std::string & json::write_config::indent() {
	return _indent;
}

std::string json::write_config::indent() const {
	return _indent;
}

void json::write_config::space(const char * str) {
	_space = str;
}

std::string & json::write_config::space() {
	return _space;
}

std::string json::write_config::space() const {
	return _space;
}

void json::write_config::space_count(size_t n) {
	_space_count = n;
}

size_t & json::write_config::space_count() {
	return _space_count;
}

size_t json::write_config::space_count() const {
	return _space_count;
}

void json::write_config::new_line(const char * str) {
	_new_line = str;
}

std::string & json::write_config::new_line() {
	return _new_line;
}

std::string json::write_config::new_line() const {
	return _new_line;
}

void json::write_config::presition(size_t n) {
	_presit = n;
}

size_t & json::write_config::presition() {
	return _presit;
}

size_t json::write_config::presition() const {
	return _presit;
}

json::json_writer::json_writer(write_config conf) : writer(nullptr), _config(conf) {
	if (_config.has_flag(write_flags::using_tabs)) {
		_config.space("\t");
	}
}

void json::json_writer::write_to_file(const json_value & json_val, const std::string & file_name) {
	write_to_file(json_val, file_name.c_str());
}

void json::json_writer::write_to_file(const json_value & json_val, const char * file_name) {
	writer = new json::file_writer(file_name);
	if (!writer->ready()) {
		throw error(0, 0, error_type::_file_not_found);
	}
	if (_config.has_flag(write_flags::using_tabs)) {
		_config.space("\t");
	}
	write(json_val);
	delete writer;
}

void json::json_writer::write_to_string(const json_value & json_val, std::string & json_string) {
	writer = new json::string_writer(json_string);
	if (_config.has_flag(write_flags::using_tabs)) {
		_config.space("\t");
	}
	write(json_val);
	delete writer;
}

void json::json_writer::write_to_stream(const json_value & json_val, std::ostream & stream) {
	writer = new json::stream_writer(stream);
	if (!writer->ready()) {
		throw error(0, 0, error_type::_stream_is_bad);
	}
	if (_config.has_flag(write_flags::using_tabs)) {
		_config.space("\t");
	}
	write(json_val);
	delete writer;
}

void json::json_writer::config(write_config conf) {
	_config = conf;
}

json::write_config & json::json_writer::config() {
	return _config;
}

json::write_config json::json_writer::config() const {
	return _config;
}

void json::json_writer::write_indent() {
	if (!_config.has_flag(write_flags::format)) {
		return;
	}
	writer->write_data(_config.new_line().c_str());
	for (size_t i = 0; i < indent_level; ++i) {
		writer->write_data(_config.indent().c_str());
	}
}

void json::json_writer::write_space() {
	if (!_config.has_flag(write_flags::format)) {
		return;
	}
	for (size_t i = 0; i < _config.space_count(); ++i) {
		writer->write_data(_config.space().c_str());
	}
}

void json::json_writer::write(const json_value & json_val) {
	switch (json_val.type()) {
	case value_type::_null:
		writer->write_data("null");
		break;
	case value_type::_bool:
		write_bool(json_val.as_bool());
		break;
	case value_type::_number:
		write_digit(json_val.as_num());
		break;
	case value_type::_string:
		write_string(json_val.as_string());
		break;
	case value_type::_array:
		write_array(*json_val.as_array());
		break;
	case value_type::_object:
		write_object(*json_val.as_object());
		break;
	}
}

void json::json_writer::write_bool(const bool & data) {
	if (!_config.has_flag(write_flags::bool_as_num)) {
		writer->write_data((data ? "true" : "false"));
	}
	else {
		writer->write_data((data ? "0" : "1"));
	}
}

void json::json_writer::write_digit(const double & data) {
	double mult = std::pow(10, _config.presition());
	double res = std::round(data*mult) / mult;
	std::string data_to_write = std::to_string(res);
	size_t dot_pos = data_to_write.find(".");
	if (dot_pos != std::string::npos) {
		data_to_write.substr(0, dot_pos + _config.presition() + 1);
		while (!data_to_write.empty() && data_to_write.back() == '0') {
			data_to_write.pop_back();
		}
		if (!data_to_write.empty() && data_to_write.back() == '.') {
			data_to_write.pop_back();
		}
	}
	writer->write_data(data_to_write.c_str());
}

void json::json_writer::write_string(const std::string & data) {
	string_coder coder;
	char quote = _config.has_flag(write_flags::single_quotes) ? '\'' : '\"';
	writer->write_data(std::string(1, quote).c_str());
	writer->write_data(coder.encode(data).c_str());
	writer->write_data(std::string(1, quote).c_str());

}

void json::json_writer::write_array(const json_array & data) {
	bool first = true;
	writer->write_data("[");
	indent_level++;
	for (size_t i = 0; i < data.size(); ++i) {
		first ? first = !first : writer->write_data(",");
		write_indent();
		write(data[i]);
	}
	if (_config.has_flag(write_flags::trailing_commas)) {
		writer->write_data(",");
	}
	indent_level--;
	write_indent();
	writer->write_data("]");
}

void json::json_writer::write_object(const json_object & data) {
	bool first = true;
	writer->write_data("{ ");
	indent_level++;
	for (auto it = data.begin(); it != data.end(); ++it) {
		first ? first = !first : writer->write_data(",");
		write_indent();
		write_string(it->first);
		writer->write_data(":");
		write_space();
		write(it->second);
	}
	if (_config.has_flag(write_flags::trailing_commas)) {
		writer->write_data(",");
	}
	indent_level--;
	write_indent();
	writer->write_data("}");
}