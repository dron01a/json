#include "error.h"

json::base_error::base_error(error_category category, size_t line, size_t col, const std::string & message)
	: _error_cat(category), _line(line), _col(col), _message(message), err(format_message()){}


size_t json::base_error::line() const{
	return _line;
}

size_t json::base_error::column() const{
	return _col;
}

json::error_category json::base_error::category() const{
	return _error_cat;
}

const char * json::base_error::what() const noexcept {
	return err.c_str();
}

std::string json::base_error::format_message(){
	std::string result = "JSON ";
	switch (_error_cat) {
	case json::error_category::io_error:
		result += "I/O error";
		break;
	case json::error_category::input_error:
		result += "input error";
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
	if (_col >= 0 && _line >= 0) {
		result += "at line " + std::to_string(_line) + " column " + std::to_string(_col);
	}
	return result;
}
