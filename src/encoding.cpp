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

json::encodings::base_decoder::base_decoder(std::unique_ptr<io_base::i_input> input) : _input(std::move(input)) {
	_position = 0;
}

char32_t json::encodings::base_decoder::current_char() {
	return _cur_char;
}

char32_t json::encodings::base_decoder::peek_char() {
	size_t temp_pos = _position;
	char32_t temp_cur_char = _cur_char;
	auto temp_buff = _buff;
	char32_t peek_char = next_char();
	_buff = std::move(temp_buff);
	_position = temp_pos;
	_cur_char = temp_cur_char;
	_buff.push_back(peek_char);
	return peek_char;
}

size_t json::encodings::base_decoder::position() {
	return _position;
}

void json::encodings::base_decoder::clear_peek_buff() {
	_buff.clear();
}

void json::encodings::base_decoder::push_buff(char32_t c) {
	_buff.push_back(c);
}

bool json::encodings::base_decoder::eof() {
	return _eof || _input->eof();
}


json::encodings::utf8_decoder::utf8_decoder(std::unique_ptr<io_base::i_input> input) 
	: base_decoder(std::move(input)) {
	_bom = skip_bom();
}

char32_t json::encodings::utf8_decoder::next_char(){
	if (!_buff.empty()) {
		_cur_char = _buff.back();
		_buff.pop_back();
		_position++;
		return _cur_char;
	}
	_cur_char = read_impl();
	return _cur_char;
}

void json::encodings::utf8_decoder::position(size_t pos){
	if (pos < 0) {
		return;
	}
	_input->seek(-(int)_position);
	_bom = skip_bom();
	_position = pos;
	_input->seek(pos);
}

char32_t json::encodings::utf8_decoder::read_impl(){
	char _c = _input->next_char();
	uint8_t first = static_cast<uint8_t>(_c);
	if (_c == std::char_traits<char>::eof()) {
		_eof = true;
		return static_cast<char32_t>(_c);
	}
	if (first <= 0x80) {
		return static_cast<char32_t>(first);
	}
	int lenght = 1;
	if ((first & 0xE0) == 0xC0) {
		lenght = 2;
	}
	if ((first & 0xF0) == 0xE0) {
		lenght = 3;
	}
	if ((first & 0xF8) == 0xF0) {
		lenght = 4;
	}
	uint8_t bytes[4] = { first };
	for (size_t i = 1; i < lenght; ++i) {
		_c = _input->next_char();
		if (_c == std::char_traits<char>::eof()) {
			throw encoding_error(encoding_error::error_code::_invalid_unicode_char, 0, 0); // числа!!!!
		}
		bytes[i] = static_cast<uint8_t>(_c);
	}
	_position += lenght;
	if ((first & 0xE0) == 0xC0) {
		uint8_t second = static_cast<uint8_t>(bytes[1]);
		if ((second & 0xC0) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (first & 0x1F) << 6 | (second & 0x3F);
		return code;
	}
	if ((first & 0xF0) == 0xE0) {
		uint8_t second = static_cast<uint8_t>(bytes[1]);
		uint8_t third = static_cast<uint8_t>(bytes[2]);
		if ((second & 0xC0) != 0x80 || (third & 0xC0) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (first & 0x0F) << 12 | (second & 0x3F) << 6 | (third & 0x3F);
		return code;
	}
	if ((first & 0xF8) == 0xF0) {
		uint8_t second = static_cast<uint8_t>(bytes[1]);
		uint8_t third = static_cast<uint8_t>(bytes[2]);
		uint8_t fourth = static_cast<uint8_t>(bytes[3]);
		if ((second & 0xC0) != 0x80 || (third & 0xC0) != 0x80 || (fourth & 0xC0) != 0x80) {
			return 0xFFFD;
		}
		uint32_t code = (first & 0x07) << 18 | (second & 0x3F) << 12 | (third & 0x3F) << 6 | (fourth & 0x3F);
		return code;
	}
	return 0xFFFD;
}

bool json::encodings::utf8_decoder::skip_bom(){
	uint8_t bom[3];
	for (size_t i = 0; i < 3; ++i) {
		int _c = _input->next_char();
		if (_c == std::char_traits<char>::eof()) {
			_input->seek(-(int)_position);
			_position = 0;
			return false;
		}
		bom[i] = static_cast<uint8_t>(_c);
	}
	if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
		_position = 3;
		return true;
	}
	_position = 0;
	_input->seek(0);
	return false;
}

json::encodings::ascii_decoder::ascii_decoder(std::unique_ptr<io_base::i_input> input) 
	: base_decoder(std::move(input)) {
	_position = 0;
}

char32_t json::encodings::ascii_decoder::next_char(){
	if (!_buff.empty()) {
		_cur_char = _buff.back();
		_buff.pop_back();
		_position++;
		return _cur_char;
	}
	uint8_t _c = _input->next_char();
	_position++;
	if ((char)_c == std::char_traits<char>::eof()) {
		_eof = true;
		_cur_char = _c;
	}
	else if (_c > 0x7F) {
		_cur_char = '?';
	}
	else if (_c < 0x20 && _c != '\t' && _c != '\n' && _c != '\r') {
		_cur_char = ' ';
	}
	else {
		_cur_char = _c;
	}
	return _cur_char;
}

void json::encodings::ascii_decoder::position(size_t pos){
	if (pos < 0) {
		return;
	}
	_input->seek(-(int)_position);
	_position = pos;
	_input->seek(pos);
}