#include "error.h"

std::ostream & json::operator<<(std::ostream & stream, json::error & err){
#ifdef RUS_ERROR_MESSAGE 
	stream << "Строка : " << err.str << std::endl << "Столбец : " << err.col << std::endl << "Сообщение : ";
#endif
#ifdef ENG_ERROR_MESSAGE 
	stream << "String : " << err.str << std::endl << "Col: " << err.col << std::endl << "Message : ";
#endif
	switch (err.type)	{
	case error_type::_file_not_found: 
		stream << FILE_NOT_FOUND;
		break;
	case error_type::_string_is_empty:
		stream << STRING_IS_EMPTY;
		break;
	case error_type::_stream_is_bad: 
		stream << STREAM_ERROR;
		break;
	case error_type::_error_token: 
		stream << ERROR_TOKEN;
		break;
	case error_type::_invalid_number: 
		stream << INVALID_NUM;
		break;
	case error_type::_invalid_number_format:
		stream << INVALID_NUM_FORMAT;
		break;
	case error_type::_invalid_string:
		stream << INVALID_STR;
		break;
	case error_type::_invalid_escape: 
		stream << INVALID_ESCAPE;
		break;
	case error_type::_invalid_unicode: 
		stream << INVALID_UNICODE;
		break;
	case error_type::_invalid_unicode_char:
		stream << INVALID_UNICODE_CHAR;
		break;
	case error_type::_invalid_unicode_low_pair: 
		stream << INVALID_UNICODE_LP;
		break;
	case error_type::_literal_error: 
		stream << LITERAL_ERROR;
		break;
	case error_type::_invalid_value: 
		stream << INVALID_VAL;
		break;
	case error_type::_invalid_object: 
		stream << INVALID_OBJECT;
		break;
	case error_type::_invalid_array_value:
		stream << INVALID_ARRAY_VALUE;
		break;
	}
	stream << std::endl;
	return stream;
}

std::string json::to_string(error & err){
	std::string res;
#ifdef RUS_ERROR_MESSAGE 
	res += "Строка : " + std::to_string(err.str) 
		+ "\nСтолбец : " + std::to_string(err.col) 
		+ "\nСообщение : ";
#endif
#ifdef ENG_ERROR_MESSAGE 
	res += "String : " + std::to_string(err.str)
		+ "\nCol : " + std::to_string(err.col)
		+ "\nMessage : ";
#endif
	switch (err.type) {
	case error_type::_file_not_found:
		res += FILE_NOT_FOUND;
		break;
	case error_type::_string_is_empty:
		res += STRING_IS_EMPTY;
		break;
	case error_type::_stream_is_bad:
		res += STREAM_ERROR;
		break;
	case error_type::_error_token:
		res += ERROR_TOKEN;
		break;
	case error_type::_invalid_number:
		res += INVALID_NUM;
		break;
	case error_type::_invalid_number_format:
		res += INVALID_NUM_FORMAT;
		break;
	case error_type::_invalid_string:
		res += INVALID_STR;
		break;
	case error_type::_invalid_escape:
		res += INVALID_ESCAPE;
		break;
	case error_type::_invalid_unicode:
		res += INVALID_UNICODE;
		break;
	case error_type::_invalid_unicode_char:
		res += INVALID_UNICODE_CHAR;
		break;
	case error_type::_invalid_unicode_low_pair:
		res += INVALID_UNICODE_LP;
		break;
	case error_type::_literal_error:
		res += LITERAL_ERROR;
		break;
	case error_type::_invalid_value:
		res += INVALID_VAL;
		break;
	case error_type::_invalid_object:
		res += INVALID_OBJECT;
		break;
	case error_type::_invalid_array_value:
		res += INVALID_ARRAY_VALUE;
		break;
	}
	res += "\n";
	return res;
}
