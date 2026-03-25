#include "json.h"

using namespace json;
using namespace json::io;
using namespace json::io_base;
using namespace json::encodings;
using namespace json::impl; 

parse_result dom_parser::from_file(const char * file_name, parse_config config){
	parse_result res;
	i_input_ptr _input = std::make_unique<file_input>(file_name);
	if (!_input->ready()) {
		res.errors.push_back(std::make_unique<io_error>(io_error::error_code::_file_not_found, 0, 0, file_name));
		return res;
	}
	parse(res, _input, config);
	return res;
}

parse_result dom_parser::from_file(const std::string & file_name, parse_config config){
	return from_file(file_name.c_str(), config);
}

parse_result dom_parser::from_stream(std::istream & stream, parse_config config){
	parse_result res;
	i_input_ptr _input = std::make_unique<stream_input>(stream);
	if (!_input->ready()) {
		res.errors.push_back(std::make_unique<io_error>(io_error::error_code::_stream_is_bad, 0, 0));
		return res;
	}
	parse(res, _input, config);
	return res;
}

parse_result dom_parser::from_string(const char * str, parse_config config) {
	parse_result res;
	i_input_ptr _input = std::make_unique<string_input>(str);
	if (!_input->ready()) {
		res.errors.push_back(std::make_unique<io_error>(io_error::error_code::_string_is_empty, 0, 0));
		return res;
	}
	parse(res, _input, config);
	return res;
}

parse_result dom_parser::from_string(const std::string & str, parse_config config) {
	return from_string(str.c_str(), config);
}

void dom_parser::parse(parse_result & res, i_input_ptr_ref input, parse_config & config) {
	dom_parser_impl _pars(input, config);
	res = _pars.parse();
}

write_result json::writer::to_file(const json_value & val, const char * file_name, write_config config) {
	write_result res;
	i_output_ptr _output = std::make_unique<file_output>(file_name);
	if (!_output->ready()) {
		res.errors.push_back(std::make_unique<io_error>(io_error::error_code::_invalid_file, 0, 0, file_name));
		return res;
	}
	writer_impl _writer(_output, config);
	_writer.write(val);
	return res;
}

write_result json::writer::to_stream(const json_value & val, std::ostream & stream, write_config config) {
	write_result res;
	i_output_ptr _output = std::make_unique<stream_output>(stream);
	if (!_output->ready()) {
		res.errors.push_back(std::make_unique<io_error>(io_error::error_code::_invalid_stream, 0, 0));
		return res;
	}
	writer_impl _writer(_output, config);
	_writer.write(val);
	return res;
}

write_result json::writer::to_string(const json_value & val, std::string & dest, write_config config) {
	write_result res;
	i_output_ptr _output = std::make_unique<string_output>(dest);
	if (!_output->ready()) {
		res.errors.push_back(std::make_unique<io_error>(io_error::error_code::_invalid_string, 0, 0));
		return res;
	}
	writer_impl _writer(_output, config);
	res = _writer.write(val);
	return res;
}

void json::xml_convert::to_file(const json_value & val, const char * file_name, write_config config) {
	i_output_ptr _output = std::make_unique<file_output>(file_name);
	if (!_output->ready()) {
		return;
	}
	convert(_output, val, config);
}

void json::xml_convert::to_stream(const json_value & val, std::ostream & stream, write_config config) {
	i_output_ptr _output = std::make_unique<stream_output>(stream);
	if (!_output->ready()) {
		return;
	}
	convert(_output, val, config);
}

std::string json::xml_convert::to_string(const json_value & val, write_config config) {
	std::string _result;
	i_output_ptr _output = std::make_unique<string_output>(_result);
	if (!_output->ready()) {
		return "";
	}
	convert(_output, val, config);
	return _result;
}

void json::xml_convert::convert(i_output_ptr_ref out, const json_value & val, write_config config) {
	config.sinax() = write_config::sinax_mode::XML;
	out->out_data("<?xml version=\"1.0\" encoding=");
	switch (config.encoding()) {
	case encoding::ascii:
		out->out_data("\"ascii\"");
		break;
	case encoding::utf8:
		out->out_data("\"utf-8\"");
		break;
	}
	out->out_data("?>");
	writer_impl _writer(out, config);
	_writer.write(val);
}
