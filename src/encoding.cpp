#include "encoding.h"

json::encodings::encoding_error::encoding_error(error_code code, size_t line, size_t col) :
	base_error(error_category::encoding_error, line, col, form_message(code))
{
	this->code = code;
}

std::string json::encodings::encoding_error::form_message(error_code code) {
	std::string result;
	switch (code){
	case json::encodings::encoding_error::error_code::_invalid_string:
		result += "invalid string";
		break;
	case json::encodings::encoding_error::error_code::_invalid_escape:
		result += "escape processing error";
		break;
	case json::encodings::encoding_error::error_code::_invalid_unicode_char:
		result += "unicode char processing error";
		break;
	case json::encodings::encoding_error::error_code::_invalid_unicode_low_pair:
		result += "unicode low pair processing error";
		break;
	}
	return result;
}


std::string json::encodings::utf8::decoder::decode(json::io_base::i_input * _src, size_t & line, size_t & col) {
	std::string result;
	result.reserve(64);
	char _c = _src->next_char();
	col++;
	while (_c != '\"') {
		switch (_c) {
		case '\0':
			throw encoding_error(encoding_error::error_code::_invalid_string, line, col);
		case '\\':
			_c = _src->next_char();
			switch (_c) {
			case '"':
				result += '\"'; break;
			case '\\':
				result += '\\'; break;
			case '/':
				result += '/'; break;
			case 'b':
				result += '\b'; break;
			case 'f':
				result += '\f'; break;
			case 'n':
				result += '\n'; break;
			case 'r':
				result += '\r'; break;
			case 't':
				result += '\t'; break;
			case 'u':
				result += read_unicode(_src, line, col); break;
			default:
				throw encoding_error(encoding_error::error_code::_invalid_escape, line, col);
			}
			break;
		default:
			result += _c;
			break;
		}
		_c = _src->next_char();
		col++;
	}
	return result;
}

std::string json::encodings::utf8::decoder::read_unicode(json::io_base::i_input * _src, size_t & line, size_t & col){
	std::string result;
	uint32_t code = 0; 
	for (size_t i = 0; i < 4; ++i) {
		char c = _src->next_char();
		col++;
		if (c == '\0') {
			throw encoding_error(encoding_error::error_code::_invalid_unicode_char, line, col);
		}
		else if (c >= '0' && c <= '9') {
			code = (code << 4) | (c - '0');
		}
		else if (c >= 'a' && c <= 'f') {
			code = (code << 4) | (c - 'a' + 10);
		}
		else if (c >= 'A' && c <= 'F') {
			code = (code << 4) | (c - 'A' + 10);
		}
		else {
			throw encoding_error(encoding_error::error_code::_invalid_unicode_char, line, col);
		}
	}
	if (code >= 0xD800 && code <= 0xDBFF) { // проверяем на наличие суррогатной пары
		uint32_t low_code = 0;
		for (size_t i = 0; i < 4; ++i) {
			char c = _src->next_char();
			col++;
			if (c == '\0') {
				throw encoding_error(encoding_error::error_code::_invalid_unicode_char, line, col);
			}
			else if (c >= '0' && c <= '9') {
				low_code = (low_code << 4) | (c - '0');
			}
			else if (c >= 'a' && c <= 'f') {
				low_code = (low_code << 4) | (c - 'a' + 10);
			}
			else if (c >= 'A' && c <= 'F') {
				low_code = (low_code << 4) | (c - 'A' + 10);
			}
			else {
				throw encoding_error(encoding_error::error_code::_invalid_unicode_char, line, col);
			}
		}
		if (low_code < 0xDC00 || low_code > 0xDFFF) {
			throw encoding_error(encoding_error::error_code::_invalid_unicode_low_pair, line, col);
		}
		code = 0x10000 + ((code - 0xD800) << 10) + (low_code - 0xDC00);
	}
	append_utf8_char(result, code);
	return result;
}

void json::encodings::utf8::decoder::append_utf8_char(std::string & _string, uint32_t code){
	if (code <= 0x7F) {
		_string += static_cast<char>(code);
	}
	else if (code <= 0x7F) {
		_string += static_cast<char>(0xC0 | (code >> 6) & 0x1F);
		_string += static_cast<char>(0x80 | code & 0x3F);
	}
	else if (code <= 0xFFFF) {
		_string += static_cast<char>(0xE0 | (code >> 12) & 0x0F);
		_string += static_cast<char>(0x80 | (code >> 6) & 0x3F);
		_string += static_cast<char>(0x80 | code & 0x3F);
	}
	else if (code <= 0x10FFFF) {
		_string += static_cast<char>(0xF0 | (code >> 18) & 0x07);
		_string += static_cast<char>(0x80 | (code >> 12) & 0x3F);
		_string += static_cast<char>(0x80 | (code >> 6) & 0x3F);
		_string += static_cast<char>(0x80 | code & 0x3F);
	}
}

void json::encodings::utf8::encoder::encode(json::io_base::i_output * _dest, const std::string & _string) {
	for (size_t i = 0; i < _string.size(); ++i) {
		uint8_t _c = static_cast<uint8_t>(_string[i]);
		switch (_c) {
		case '\"': _dest->out_data("\\\""); break;
		case '\\': _dest->out_data("\\\\"); break;
		case '/': _dest->out_data("\\/"); break;
		case '\b': _dest->out_data("\\b"); break;
		case '\f': _dest->out_data("\\f"); break;
		case '\n': _dest->out_data("\\n"); break;
		case '\r': _dest->out_data("\\r"); break;
		case '\t': _dest->out_data("\\t"); break;
		default:
			if (_c < 32) {
				append_hex_4(_dest, _c);
			}
			else if (_c < 128) {
				_dest->out_data(_c);
			}
			else {
				int len = get_utf8_len(_c);
				if (i + len > _string.size()) {
					_dest->out_data('?');
					continue;
				}
				uint32_t code = parse_utf8(_string, i, len);
				if (code == 0xFFFD) {
					_dest->out_data('?');
					continue;
				}
				else if (code <= 0xFFFF){
					if (code < 32 || code == '"' || code == '\\' || code == '/' || code == '\b' 
						|| code == '\f' || code == '\n' || code == '\r' || code == '\t') {
						_dest->out_data(static_cast<char>(code));
					}
					else if (code < 128) {
						_dest->out_data(static_cast<char>(code));
					}
					else {
						append_hex_4(_dest, static_cast<uint16_t>(code));
					}
				}
				else {
					code -= 0x10000;
					uint16_t high = static_cast<uint16_t>((code >> 10) + 0xD800);
					uint16_t low = static_cast<uint16_t>((code & 0x3FF) + 0xDC00);
					append_hex_4(_dest, high);
					append_hex_4(_dest, low);
				}
			}
			break;
		}
	}
}

inline void json::encodings::utf8::encoder::append_hex_4(json::io_base::i_output * _dest, uint16_t value){
	static const char hex_digit[] = "0123456789ABCDEF";
	_dest->out_data("\\u");
	_dest->out_data(hex_digit[(value >> 12) & 0xF]);
	_dest->out_data(hex_digit[(value >> 8) & 0xF]);
	_dest->out_data(hex_digit[(value >> 4) & 0xF]);
	_dest->out_data(hex_digit[value & 0xF]);
}

inline int json::encodings::utf8::encoder::get_utf8_len(unsigned char fb){
	if ((fb & 0x80) == 0x00) {
		return 1;
	}
	if ((fb & 0xE0) == 0xC0) {
		return 2;
	}
	if ((fb & 0xF0) == 0xE0) {
		return 3;
	}
	if ((fb & 0xF8) == 0xF0) {
		return 4;
	}
	return 1;
}

inline uint32_t json::encodings::utf8::encoder::parse_utf8(const std::string & str, size_t & pos, int len){
	if (len == 0) {
		return 0;
	}
	uint8_t first = static_cast<unsigned char>(str[pos]);
	if (first <= 0x7F) {
		return first;
	}
	if ((first & 0xE0) == 0xC0) {
		if (pos + 1 >= str.size()) {
			return 0xFFFD;
		}
		uint8_t second = static_cast<uint8_t>(str[pos + 1]);
		if ((second & 0xC0) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (first & 0x1F) << 6 | (second & 0x3F);
		pos += 1;
		return code;
	}
	if ((first & 0xF0) == 0xE0) {
		if (pos + 2 >= str.size()) {
			return 0xFFFD;
		}
		uint8_t second = static_cast<uint8_t>(str[pos + 1]);
		uint8_t third = static_cast<uint8_t>(str[pos + 2]);
		if ((second & 0xC0) != 0x80 || (third & 0xC0) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (first & 0x0F) << 12 | (second & 0x3F) << 6 | (third & 0x3F);
		pos += 2;
		return code;
	}
	if ((first & 0xF8) == 0xF0) {
		if (pos + 3 >= str.size()) {
			return 0xFFFD;
		}
		uint8_t second = static_cast<uint8_t>(str[pos + 1]);
		uint8_t third = static_cast<uint8_t>(str[pos + 2]);
		uint8_t fourth = static_cast<uint8_t>(str[pos + 3]);
		if ((second & 0xC0) != 0x80 || (third & 0xC0) != 0x80 || (fourth & 0xC0) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (first & 0x07) << 18 | (second & 0x3F) << 12 | (third & 0x3F) << 6 | (fourth & 0x3F);
		pos += 3;
		return code;
	}
	return 0xFFFD;
}


std::string json::encodings::ascii::decoder::decode(json::io_base::i_input * _src, size_t & line, size_t & col) {
	std::string result;
	result.reserve(64);
	char _c = _src->next_char();
	col++;
	while (_c != '\"') {
		switch (_c) {
		case '\0':
			throw encoding_error(encoding_error::error_code::_invalid_string, line, col);
		case '\\':
			_c = _src->next_char();
			switch (_c) {
			case '"':
				_c = '\"'; break;
			case '\\':
				_c = '\\'; break;
			case '/':
				_c = '/'; break;
			case 'b':
				_c = '\b'; break;
			case 'f':
				_c = '\f'; break;
			case 'n':
				_c = '\n'; break;
			case 'r':
				_c = '\r'; break;
			case 't':
				_c = '\t'; break;
			case '\0':
				throw encoding_error(encoding_error::error_code::_invalid_escape, line, col);
			}
			if (static_cast<uint8_t>(_c) > 0x7F) {
				result.push_back('?');
			}
			else {
				result.push_back(_c);
			}
			break;
		default:
			if (static_cast<uint8_t>(_c) > 0x7F) {
				result.push_back('?');
			}
			else if (static_cast<uint8_t>(_c) < 32 && _c != '\t' && _c != '\n' && _c != '\r') {
				result.push_back(' ');
			}
			else {
				result.push_back(_c);
			}
			break;
		}
		_c = _src->next_char();
		col++;
	}
	return result;
}

void json::encodings::ascii::encoder::encode(json::io_base::i_output * _dest, const std::string & _string) {
	for (size_t i = 0; i < _string.size(); ++i) {
		uint8_t _c = static_cast<uint8_t>(_string[i]);
		switch (_c) {
		case '\"': _dest->out_data("\\\""); break;
		case '\\': _dest->out_data("\\\\"); break;
		case '/': _dest->out_data("\\/"); break;
		case '\b': _dest->out_data("\\b"); break;
		case '\f': _dest->out_data("\\f"); break;
		case '\n': _dest->out_data("\\n"); break;
		case '\r': _dest->out_data("\\r"); break;
		case '\t': _dest->out_data("\\t"); break;
		default:
			if (static_cast<uint8_t>(_c) > 0x7F) {
				_dest->out_data('?');
			}
			else if (static_cast<uint8_t>(_c) < 32 && _c != '\t' && _c != '\n' && _c != '\r') {
				_dest->out_data(' ');
			}
			else {
				_dest->out_data(_c);
			}
			break;
		}
	}
}

