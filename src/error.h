#ifndef __DRONJSON__ERROR__
#define __DRONJSON__ERROR__

#include <string>

namespace json {

	// типы ошибок
	enum class error_type {
		_none = 0,
		_file_not_found,
		_string_is_empty,
		_stream_is_bad,
		_error_token,
		_invalid_number,
		_invalid_number_format,
		_invalid_string,
		_invalid_escape,
		_invalid_unicode,
		_invalid_unicode_char,
		_invalid_unicode_low_pair,
		_literal_error,
		_invalid_value,
		_invalid_object,
		_invalid_array_value
	};

	// структура ошибки
	struct error {
		error() {}
		error(size_t _c, size_t _s, error_type _e) : col(_c), str(_s), type(_e) {}
		size_t col = 0, str = 0;
		error_type type = error_type::_none;
	};

}


#endif