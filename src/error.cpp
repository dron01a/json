#include "error.h"

json::base_error::base_error(size_t line, size_t col, const std::string & message)
	: _line(line), _col(col), _message(message){}

size_t json::base_error::line() const{
	return _line;
}

size_t json::base_error::column() const{
	return _col;
}

const char * json::base_error::what() const noexcept {
	return format_message().c_str();
}

std::string json::base_error::format_message() const{
	std::string result = "JSON ";
	switch (_error_cat) {
	case json::error_category::io_error:
		result += "I/O error";
		break;
	case json::error_category::encoding_error:
		result += "encoding error";
		break;
	case json::error_category::parse_error:
		result += "parsing error";
		break;
	case json::error_category::document_error:
		result += "document error";
		break;
	case json::error_category::validation_error:
		result += "validation error";
		break;
	case json::error_category::value_error:
		result += "value error";
		break;
	}
	result += ": " + _message + "\n";
	if (_col > 0 && _line > 0) {
		result += "at line " + std::to_string(_line) + " column " + std::to_string(_col);
	}
	return result;
}
