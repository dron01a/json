#include "encoding.h"

std::string json::encodings::utf8::decoder::decode(json::io_base::i_input * _src){
	std::string result;
	result.reserve(64);
	char _c = _src->next_char();
	switch (_c) {
	case '"':
		return std::string(1, '\"');
	case '\\':
		return std::string(1, '\\');
	case '/':
		return std::string(1, '/');
	case 'b':
		return std::string(1, '\b');
	case 'f':
		return std::string(1, '\f');
	case 'n':
		return std::string(1, '\n');
	case 'r':
		return std::string(1, '\r');
	case 't':
		return std::string(1, '\t');
	case 'u':
	//	return utf32_to_utf8(read_unicode(reader, line, col));
	case '\0':
	//	throw error(col, line, error_type::_invalid_string);
	default:
	//	throw error(col, line, error_type::_invalid_escape);
	}
	/*for (size_t i = 0; i < _src.size(); ++i) {
		if (_src[i] == '\\') {
			i++;
			switch (_src[i]){

			default
				break;
			}
		}
	}*/

	return result;
}
