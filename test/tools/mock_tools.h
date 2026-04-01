#ifndef _MOCK_TOOLS_
#define _MOCK_TOOLS_

#include <vector>

#include "encoding.h"

using char32_vect = std::vector<char32_t>;

class mock_decoder : public json::core::io::encodings::i_decoder  {
public:

	// конструктор 
	mock_decoder(const char32_vect & data) : _data(data), _pos(0) {}

	char32_t next_char() override {
		if (!_buff.empty()) {
			char32_t _c = _buff.back();
			_buff.pop_back();
			return _c;
		}
		if (_pos >= _data.size()) {
			return std::char_traits<char>::eof();
		}
		return _data[_pos++];
	}

	char32_t current_char() override {
		if (!_buff.empty()) {
			return _buff.back();
		}
		if (_pos >= _data.size()) {
			return std::char_traits<char>::eof();
		} 
		if (_pos == 0) {
			return _data[0];
		}
		return _data[_pos - 1];
	}

	char32_t peek_char() override {
		if (!_buff.empty()) {
			return _buff.back();
		}
		if (_pos >= _data.size()) {
			return std::char_traits<char>::eof();
		}
		return _data[_pos];
	}

	void clear_peek_buff() override {
		_buff.clear();
	}

	void push_buff(char32_t c) override {
		_buff.push_back(c);
	}

	json::encoding type() {
		return json::encoding::ascii;
	}

private:

	char32_vect _buff; // буфер для символов 
	char32_vect _data; // данные
	size_t _pos; // позиция 

};

char32_vect string_to_char32(const std::string & data) {
	char32_vect _result;
	for (auto _c : data) {
		_result.push_back(static_cast<char32_t>(_c));
	}
	return _result;
}

json::core::io::encodings::i_decoder_ptr make_decoder(const std::string & data) {
	return std::make_unique<mock_decoder>(string_to_char32(data));
}

#endif // !_MOCK_TOOLS_

