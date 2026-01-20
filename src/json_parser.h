#ifndef __DRONJSON__PARSER__
#define __FRONJSON__PARSER__

#include "error.h"
#include "json_reader.h"
#include "json_value.h"

namespace json {

	// типы токенов
	enum class token_type {
		_space = 0, // в том числе и табуляции
		_colon, // : 
		_comma, // , 
		_open_curly_brt, // {
		_close_curly_brt, // }
		_open_square_brt, // [
		_close_square_brt, // ]
		_escape_begin, // \ 
		_mark, // "
		_string, // буквы и тд
		_number, // 0 - 9, числа  
		_unknown, // неизвестный символ
		_null, // ничего, 
		_true,
		_false,
		_key,
		_end // конец данных
	};

	// токен
	struct token {
		token() { num_data = 0; };
		token(token_type _t) : _type(_t) { num_data = 0; };
		token(const token & t) {
			_type = t._type;
			switch (t._type) {
			case token_type::_number:
				string_data = "";
				num_data = t.num_data;
				break;
			case token_type::_string:
				num_data = 0;
				string_data = t.string_data;
				break;
			default:
				num_data = 0;
				string_data = "";
				break;
			}
		}
		token & operator=(const token & t) {
			_type = t._type;
			switch (t._type) {
			case token_type::_number:
				num_data = t.num_data;
				break;
			case token_type::_string:
				string_data = t.string_data;
				break;
			default:
				num_data = 0;
				string_data = "";
				break;
			}
			return *this;
		}
		~token() {  };
		std::string string_data;
		double num_data;
		token_type _type; // тип 
	};

	// извлекает токены
	class tokenizer {
	public:
		// конструтор
		tokenizer(i_reader * _reader);

		// деструтор класса 
		~tokenizer();

		// возвращает флаг готовности
		bool ready() const;

		// возвращает следущий токен 
		token & get_next_token();

		// возвращает текущий токен
		token & get_last_token();

		size_t col = 0;
		size_t str = 0;
	private:
		// пропускает пробелы
		void skip_space();

		// получает значения между кавычками
		std::string string_parse();

		// получает число
		double number_parse();

		// парсит литералов
		bool literal_parse(std::string _literal);

		token cur_token; // текущий токен
		i_reader * reader; // для чтения данных

	};

	// класс парсера
	class json_parser {
	public:
		// конструктор класса
		json_parser();

		// деструктор класса
		~json_parser();

		// загрузка из файла
		json_value load_from_file(const std::string & file_name);
		json_value load_from_file(const char * file_name);

		// загрузка из строки
		json_value load_from_string(const std::string & json_string);
		json_value load_from_string(const char * json_string);

		// загрузка из потока
		json_value load_from_stream(std::istream & stream);

		// возвращает последнюю полученную ошибку
		error get_last_error();

		// общая функция для загрузки данных (на случай создания пользовательских i_reader)
		json_value load_data(i_reader * r, error_type err);

	private:
	
		// запускает парсинг
		json_value parse(tokenizer & _tokenizer);

		// парсинг json-начения
		json_value parse_json_value(tokenizer & _tokenizer);

		// парсинг массива 
		json_value parse_array(tokenizer & _tokenizer);

		// парсинг объекта
		json_value parse_object(tokenizer & _tokenizer);

		error last_error; // последняя полученная ошибка
	};

}

#endif