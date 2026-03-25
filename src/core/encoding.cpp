#include "core\encoding.h"

using namespace json;
using namespace json::io_base;
using namespace json::encodings;

base_decoder::base_decoder(i_input_ptr_ref input) : _input(input) {
	_position = 0;
}

char32_t base_decoder::current_char() {
	if (!_buff.empty()) {
		return _buff.back();
	}
	return _cur_char;
}

char32_t base_decoder::peek_char() {
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

size_t base_decoder::position() {
	return _position;
}

void base_decoder::clear_peek_buff() {
	_buff.clear();
}

void base_decoder::push_buff(char32_t c) {
	_buff.push_back(c);
}

bool base_decoder::eof() {
	return _eof || _input->eof();
}

encoding json::encodings::base_decoder::type() {
	return _type;
}

utf8_decoder::utf8_decoder(io_base::i_input_ptr_ref input) : base_decoder(input) {
	_type = encoding::utf8;
	_bom = skip_bom();
}

char32_t utf8_decoder::next_char(){
	if (!_buff.empty()) {
		_cur_char = _buff.back();
		_buff.pop_back();
	//	_position++;
		return _cur_char;
	}
	_cur_char = read_impl();
	return _cur_char;
}

void utf8_decoder::position(size_t pos){
	if (pos < 0) {
		return;
	}
	_input->seek(-(int)_position);
	_bom = skip_bom();
	_position = pos;
	_input->seek(pos);
}

char32_t utf8_decoder::read_impl() {
	int _c = _input->next_char();
	if (_c == std::char_traits<char>::eof() || _input->eof()) {
		_eof = true;
		return static_cast<char32_t>(_c);
	}
	uint8_t first = static_cast<uint8_t>(_c);
	_position++;
	if (first <= 0x7F) {
		return static_cast<char32_t>(first);
	}
	int length = 0;
	uint8_t mask = 0;
	uint32_t code = 0;
	if ((first & 0xE0) == 0xC0) {
		length = 2;
		code = first & 0x1F;
	}
	else if ((first & 0xF0) == 0xE0) {
		length = 3;
		code = first & 0x0F;
	}
	else  if ((first & 0xF8) == 0xF0) {
		length = 4;
		code = first & 0x07;
	}
	else {
		_position++;
		return 0xFFFD;
	}
	for (size_t i = 1; i < length; ++i) {
		_c = _input->next_char();
		if (_c == std::char_traits<char>::eof()) {
			return std::char_traits<char>::eof();
		}
		code = (code << 6) | (_c & 0x03F);
	}
	if ((length == 2 && code < 0x80) || (length == 3 && code < 0x800) || (length == 4 && code < 0x10000)) {
		return 0xFFFD;
	}
	return static_cast<char32_t>(code);
}

bool utf8_decoder::skip_bom(){
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
	_input->seek(-3);
	return false;
}

ascii_decoder::ascii_decoder(i_input_ptr_ref input) : base_decoder(input) {
	_type = encoding::ascii;
}

char32_t ascii_decoder::next_char(){
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

void ascii_decoder::position(size_t pos){
	if (pos < 0) {
		return;
	}
	_input->seek(-(int)_position);
	_position = pos;
	_input->seek(pos);
}

utf8_encoder::utf8_encoder(i_output_ptr_ref dest) : _output(dest) {}

void json::encodings::utf8_encoder::encode_code(char32_t code) {
	if (code > 0x10FFFF | (code >= 0xD800 && code <= 0xDFFF)) {
		code = 0xFFFD;
	}
	if (code <= 0x7F) {
		_output->out_data(static_cast<char>(code));
	}
	else if (code <= 0x7FF) {
		_output->out_data(static_cast<char>(0xC0 | (code >> 6) & 0x1F));
		_output->out_data(static_cast<char>(0x80 | code & 0x3F));
	}
	else if (code <= 0xFFFF) {
		_output->out_data(static_cast<char>(0xE0 | (code >> 12) & 0x0F));
		_output->out_data(static_cast<char>(0x80 | (code >> 6) & 0x3F));
		_output->out_data(static_cast<char>(0x80 | code & 0x3F));
	}
	else {
		_output->out_data(static_cast<char>(0xF0 | (code >> 18) & 0x07));
		_output->out_data(static_cast<char>(0x80 | (code >> 12) & 0x3F));
		_output->out_data(static_cast<char>(0x80 | (code >> 6) & 0x3F));
		_output->out_data(static_cast<char>(0x80 | code & 0x3F));
	}
}

void json::encodings::utf8_encoder::encode_string(const std::string & string) {
	uint8_t first;
	char32_t code;
	for (size_t i = 0; i < string.size(); ++i) {
		switch (string[i]) {
		case '\"':
			_output->out_data("\\\"");
			break;
		case '\\':
			_output->out_data("\\\\");
			break;
		case '/':
			_output->out_data("\\/");
			break;
		case '\b':
			_output->out_data("\\b");
			break;
		case '\f':
			_output->out_data("\\f");
			break;
		case '\n':
			_output->out_data("\\n");
			break;
		case '\r':
			_output->out_data("\\r");
			break;
		case '\t':
			_output->out_data("\\t");
			break;
		default:
			first = string[i];
			code = 0;
			if (first <= 0x7F) {
				code = first;
			}
			else if ((first & 0xE0) <= 0xC0) {
				i++;
				code = (first & 0x1F) << 6 | (string[i] & 0x3F);
			}
			else if ((first & 0xF0) == 0xE0) {
				i += 2;
				code = (first & 0x0F) << 12 | (string[i - 1] & 0x3F) << 6 | (string[i] & 0x3F);
			}
			else if ((first & 0xF8) == 0xF0) {
				i += 3;
				code = (first & 0x07) << 18 | (string[i - 2] & 0x3F) << 12 | (string[i - 1] & 0x3F) << 6 | (string[i] & 0x3F);
			}
			else {
				code = 0xFFFD;
			}
			encode_code(code);
			break;
		}
	}
}

void json::encodings::utf8_encoder::add_bom() {
	_output->out_data('\xEF');
	_output->out_data('\xBB');
	_output->out_data('\xBF');
}

io_base::i_output_ptr_ref json::encodings::utf8_encoder::output() {
	return _output;
}

json::encodings::ascii_encoder::ascii_encoder(io_base::i_output_ptr_ref dest) : _output(dest) {}

void json::encodings::ascii_encoder::encode_code(char32_t code) {
	if (code <= 127) {
		if (code >= 32 || code == '\t' || code == '\r' || code == '\n') {
			_output->out_data(static_cast<char>(code));
			return;
		}
	}
	_output->out_data('?'); // добавить таблицу заменяемых символов ???
}

void json::encodings::ascii_encoder::encode_string(const std::string & string) {
	for (size_t i = 0; i < string.size(); ++i) {
		switch (string[i]) {
		case '\"':
			_output->out_data("\\\"");
			break;
		case '\\':
			_output->out_data("\\\\");
			break;
		case '/':
			_output->out_data("\\/");
			break;
		case '\b':
			_output->out_data("\\b");
			break;
		case '\f':
			_output->out_data("\\f");
			break;
		case '\n':
			_output->out_data("\\n");
			break;
		case '\r':
			_output->out_data("\\r");
			break;
		case '\t':
			_output->out_data("\\t");
			break;
		default:
			encode_code(string[i]);
			break;
		}
	}
}

io_base::i_output_ptr_ref json::encodings::ascii_encoder::output() {
	return _output;
}

encodings::i_decoder_ptr json::make_decoder(encoding enc, io_base::i_input_ptr_ref input) {
	switch (enc){
	case json::encoding::ascii:
		return std::make_unique<ascii_decoder>(input);
	case json::encoding::utf8:
		return std::make_unique<utf8_decoder>(input);
	}

}

encodings::i_encoder_ptr json::make_encoder(encoding enc, io_base::i_output_ptr_ref output) {
	switch (enc) {
	case json::encoding::ascii:
		return std::make_unique<ascii_encoder>(output);
	case json::encoding::utf8:
		return std::make_unique<utf8_encoder>(output);
	};
}
