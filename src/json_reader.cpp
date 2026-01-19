#include "json_reader.h"

#include <cctype>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdio.h>

json::file_reader::file_reader(const std::string & file_name) {
	file.open(file_name, std::ios::binary);
}

json::file_reader::file_reader(const char * file_name) {
	file.open(file_name, std::ios::in | std::ios::binary);
}

json::file_reader::~file_reader() {
	if (file.is_open()) {
		file.close();
	}
}

char json::file_reader::get_next_char() {
	cur_char = file.get();
	if (file.eof()) {
		cur_char = '\0';
	}
	return cur_char;
}

char & json::file_reader::get_last_char() {
	return cur_char;
}

void json::file_reader::step_back(int n) {
	file.seekg(-n, std::ios::cur);
}

bool json::file_reader::ready() {
	return file.good() && file.is_open();
}

json::string_reader::string_reader(const std::string & string) {
	str = string;
}

json::string_reader::string_reader(const char * string) {
	str = string;
}

char json::string_reader::get_next_char() {
	if (position < str.size()) {
		return str[position++];
	}
	return '\0';
}

char & json::string_reader::get_last_char() {
	if (position < str.size() && position > 0) {
		return str[position - 1];
	}
	if (position == 0) {
		return str[0];
	}
	return str[str.size() - 1];
}

void json::string_reader::step_back(int n) {
	if (position - n >= 0) {
		position -= n;
	}
}

bool json::string_reader::ready() {
	return !str.empty() && position < str.size();
}

json::stream_reader::stream_reader(std::istream & stream) {
	this->stream = &stream;
}

json::stream_reader::~stream_reader() {
	stream = nullptr;
}

char json::stream_reader::get_next_char() {
	*stream >> cur_char;
	if (stream->eof()) {
		cur_char = '\0';
	}
	return cur_char;
}

char & json::stream_reader::get_last_char() {
	return cur_char;
}

void json::stream_reader::step_back(int n) {
	stream->seekg(-n, std::ios::cur);
}

bool json::stream_reader::ready() {
	return stream->good();
}


std::string json::string_coder::decode(i_reader * reader, size_t & line, size_t & col) {
	char cur_char = reader->get_next_char();
	col++;
	switch (cur_char) {
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
		return utf32_to_utf8(read_unicode(reader, line, col));
	case '\0':
		throw error(col, line, error_type::_invalid_string);
	default:
		throw error(col, line, error_type::_invalid_escape);
	}
}

std::string json::string_coder::encode(const std::string & src_string) {
	std::ostringstream result;
	for (size_t i = 0; i < src_string.size(); ++i) {
		uint8_t c = static_cast<uint8_t>(src_string[i]);
		switch (c) {
		case '\"':
			result << "\\\"";
			break;
		case '\\':
			result << "\\\\";
			break;
		case '/':
			result << "\\/";
			break;
		case '\b':
			result << "\\b";
			break;
		case '\f':
			result << "\\f";
			break;
		case '\n':
			result << "\\n";
			break;
		case '\r':
			result << "\\r";
			break;
		case '\t':
			result << "\\t";
			break;
		default:
			if (c < 0x20 || c == 0x7F) {
				result << unicode_escape(c);
			}
			else {
				size_t len = get_utf8_len(c);
				for (size_t j = 0; j < len && i < src_string.size(); ++j) {
					result << src_string[i++];
				}
				i--;
			}
			break;
		}
	}
	return result.str();
}

char32_t json::string_coder::read_unicode(i_reader * reader, size_t & line, size_t & col) {
	std::string result;
	char cur_char;
	for (size_t i = 0; i < 4; ++i) {
		cur_char = reader->get_next_char();
		col++;
		if (cur_char == '\0') {
			throw error(col, line, error_type::_invalid_string);
		}
		if (!std::isxdigit(static_cast<uint8_t>(cur_char))) {
			throw error(col, line, error_type::_invalid_unicode_char);
		}
		result += cur_char;
	}
	try {
		uint32_t code = std::stoul(result, nullptr, 16);
		if (code > 0x10FFFF) {
			throw error(col, line, error_type::_invalid_unicode);
		}
		if (code >= 0xD800 && code <= 0xDBFF) {
			char32_t low_pair = read_low_pair(reader, line, col);
			return 0x10000 + ((code - 0xD800) << 10) + (low_pair - 0xDC00);
		}
		return static_cast<char32_t>(code);
	}
	catch (std::exception & e) {
		throw error(col, line, error_type::_invalid_unicode);
	}
}

char32_t json::string_coder::read_low_pair(i_reader * reader, size_t & line, size_t & col) {
	char cur_char = reader->get_next_char();
	col++;
	if (cur_char != '\\') {
		throw error(col, line, error_type::_invalid_unicode_low_pair);
	}
	cur_char = reader->get_next_char();
	col++;
	if (cur_char == 'u') {
		std::string hec_str;
		for (size_t i = 0; i < 4; ++i) {
			cur_char = reader->get_next_char();
			col++;
			if (cur_char == '\0') {
				throw error(col, line, error_type::_invalid_string);
			}
			if (!std::isxdigit(static_cast<uint8_t>(cur_char))) {
				throw error(col, line, error_type::_invalid_unicode_char);
			}
			hec_str += cur_char;
		}
		uint32_t code = std::stoul(hec_str, nullptr, 16);
		if (code >= 0xD800 && code <= 0xD8FF) {
			throw error(col, line, error_type::_invalid_unicode);
		}
		return code;
	}
	throw error(col, line, error_type::_invalid_unicode_low_pair);
}

std::string json::string_coder::utf32_to_utf8(char32_t code) {
	std::string result;
	if (code <= 0x7F) {
		result += static_cast<char>(code);
	}
	else if (code <= 0x7F) {
		result += static_cast<char>(0xC0 | (code >> 6) & 0x1F);
		result += static_cast<char>(0x80 | code & 0x3F);
	}
	else if (code <= 0xFFFF) {
		result += static_cast<char>(0xE0 | (code >> 12) & 0x0F);
		result += static_cast<char>(0x80 | (code >> 6) & 0x3F);
		result += static_cast<char>(0x80 | code & 0x3F);
	}
	else if (code <= 0x10FFFF) {
		result += static_cast<char>(0xF0 | (code >> 18) & 0x07);
		result += static_cast<char>(0x80 | (code >> 12) & 0x3F);
		result += static_cast<char>(0x80 | (code >> 6) & 0x3F);
		result += static_cast<char>(0x80 | code & 0x3F);
	}
	return result;
}

std::string json::string_coder::unicode_escape(uint32_t code) {
	std::ostringstream result;
	if (code <= 0xFFFF) {
		result << "\\u" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << code;
	}
	else if (code <= 0x10FFFF) {
		code -= 0x10000;
		uint16_t high = 0xD800 + (code >> 10);
		uint16_t low = 0xDC00 + (code & 0x3FF);
		result << "\\u" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << high
			<< "\\u" << std::setw(4) << low;
	}
	else {
		result << "\\uFFFD";
	}
	return result.str();;
}

size_t json::string_coder::get_utf8_len(unsigned char fb) {
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

uint32_t json::string_coder::decode_utf8(const std::string & str, size_t & position) {
	uint8_t f = static_cast<uint8_t>(str[position]);
	if (f < 0x80) {
		return f;
	}
	if ((f & 0xE0) == 0xE0) {
		if (position + 1 >= str.size()) {
			return 0xFFFD;
		}
		uint8_t s = static_cast<uint8_t>(str[position + 1]);
		if ((s & 0xC8) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (f & 0x1F) << 6 | (s & 0x3F);
		position += 1;
		return code;
	}
	if ((f & 0xE0) == 0xE0) {
		if (position + 2 >= str.size()) {
			return 0xFFFD;
		}
		uint8_t s = static_cast<uint8_t>(str[position + 1]);
		uint8_t t = static_cast<uint8_t>(str[position + 2]);
		if ((s & 0xC8) != 0x80 || (t & 0xC8) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (f & 0x0F) << 12 | (s & 0x3F) << 6 | (t & 0x3F);
		position += 2;
		return code;
	}
	if ((f & 0xE0) == 0xF0) {
		if (position + 3 >= str.size()) {
			return 0xFFFD;
		}
		uint8_t s = static_cast<uint8_t>(str[position + 1]);
		uint8_t t = static_cast<uint8_t>(str[position + 2]);
		uint8_t fo = static_cast<uint8_t>(str[position + 3]);
		if ((s & 0xC8) != 0x80 || (t & 0xC8) != 0x80 || (fo & 0xC8) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (f & 0x07) << 18 | (s & 0x3F) << 12 | (t & 0x3F) << 6 | (fo & 0x3F);
		position += 3;
		return code;
	}
	return 0xFFFD;
}
