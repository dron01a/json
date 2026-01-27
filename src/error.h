#ifndef __DRONJSON__ERROR__
#define __DRONJSON__ERROR__

#include <string>

#define RUS_ERROR_MESSAGE 

#ifdef  RUS_ERROR_MESSAGE

#define NONE
#define FILE_NOT_FOUND "файл не найден"
#define STRING_IS_EMPTY "исходна€ строка пуста€"
#define STREAM_ERROR "ошибка потока ввода вывода"
#define ERROR_TOKEN "ошибочный символ/строка"
#define INVALID_NUM "ошибка в записи числа"
#define INVALID_NUM_FORMAT "неверный формат числа"
#define INVALID_STR "ошибка в записи строки"
#define INVALID_ESCAPE "неверна€ escape-поледовательность"
#define INVALID_UNICODE "неверна€ unicode-поледовательность"
#define INVALID_UNICODE_CHAR "неверный unicode-символ"
#define INVALID_UNICODE_LP "неверный unicode-символ нижн€€ пара"
#define LITERAL_ERROR "ошибка при чтении ключевого слова"
#define INVALID_VAL "ошибка обработки json-значени€"
#define INVALID_OBJECT "ошибка обработки json-объекта"
#define INVALID_ARRAY_VALUE "ошибка обработки члена json-массива"

#define IS_NOT_OBJ_ITR "итератор не дл€ объекта"

#endif // ! RUS_ERROR_MESSAGE

#ifdef  ENG_ERROR_MESSAGE

#define NONE
#define FILE_NOT_FOUND "file not found"
#define STRING_IS_EMPTY "string is empty"
#define STREAM_ERROR "stream is bad"
#define ERROR_TOKEN "base_error token"
#define INVALID_NUM "invalid number"
#define INVALID_NUM_FORMAT "invalid number format"
#define INVALID_STR "invalid string"
#define INVALID_ESCAPE "invalid escape char"
#define INVALID_UNICODE "invalid unicode string"
#define INVALID_UNICODE_CHAR "invalid unicode char"
#define INVALID_UNICODE_LP "invalid unicode low pair"
#define LITERAL_ERROR "base_error from read key-word"
#define INVALID_VAL "json-value processing base_error"
#define INVALID_OBJECT "json-object processing base_error"
#define INVALID_ARRAY_VALUE "json-array value processing base_error"

#endif // ! ENG_ERROR_MESSAGE

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
		_invalid_array_value,
		_parse_error,
		
		_is_not_object_iterator
	};

	// структура ошибки
	struct base_error {
		base_error() {}
		base_error(size_t _c, size_t _s, error_type _e) : col(_c), str(_s), type(_e) {}
		size_t col = 0, str = 0;
		error_type type = error_type::_none;
	};

	// дл€ вывода ошибок
	std::ostream & operator<<(std::ostream & stream, json::base_error & err);

	std::string to_string(base_error & err);
}


#endif