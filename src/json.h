#ifndef __DRONJSON__
#define __DRONJSON__

#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace json {

	class json_value;

	using json_object = std::map<std::string, json_value>;
	using json_array = std::vector<json_value>;

	// типы ошибок
	enum class error_type {
		_error_token = 0, 
		_invalid_number, 
		_invalid_number_format,
		_literal_error,
	};

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

	enum class value_type{
		_null, 
		_bool,
		_number,
		_string,
		_array, 
		_object
	};

	// токен
	struct token {
		std::string data; // данные
		token_type _type; // тип 
	};

	// класс занчения 
	class json_value {
	public:
		// конструтор класса
		json_value();
		json_value(bool data);
		json_value(double data);
		json_value(const std::string & string);
		json_value(const char * string);
		json_value(json_array data);
		json_value(json_object data);
		json_value(const json_value & val);
		json_value(json_value && val);

		json_value & operator=(json_value & jval);
		json_value & operator=(json_value && jval);

		// деструктор класса
		~json_value();

		// для получения булевого значения 
		bool & as_bool();
		void as_bool(bool & val);

		// для получения числового значения
		double & as_num();
		void as_num(double num);

		// для получения строки
		std::string & as_string();
		void as_string(const std::string & string);
		void as_string(const char * string);

		// для получения массива
		json_array * as_array();
		void as_array(json_array array);

		// для получения объекта
		json_object * as_object();
		void as_object(json_object object);

		// возвращает тип
		value_type type() const;

	private:

		// очистка данных в зависимости от типа
		void clear_data();

		// перемещение данных 
		void copy_data(const json_value & val);

		// копирование данных
		void move_data(json_value && val);

		value_type _type = value_type::_null; // тип

		// данные
		union {
			bool bool_data; 
			double num_data;
			std::string  * str_data;
			json_object * object_data;
			json_array * array_data;
		};
	};

	// итерфейс чтения символов 
	class i_reader {
	public:
		virtual ~i_reader() = default;
		virtual char get_next_char() = 0;
		virtual char & get_last_char() = 0;
		virtual bool ready() = 0;
		virtual void step_back(int n) = 0;
	};

	// чтение из файла 
	class file_reader : public i_reader{
	public:

		// конструктор
		explicit file_reader(const std::string & file_name);
		explicit file_reader(const char * file_name);

		// деструтор
		~file_reader();
		
		// возвращает следующий символ
		char get_next_char();

		// возвращает предыдущий символ
		char & get_last_char();

		// делает шаг назад
		void step_back(int n);

		// проверка на готовность к работе
		bool ready();
	private:
		std::ifstream file;
		char cur_char;
	};

	// чтение из файла 
	class string_reader : public i_reader {
	public:

		// конструктор
		explicit string_reader(const std::string & string);
		explicit string_reader(const char * string);

		// деструтор
		~string_reader() {} ;

		// возвращает предыдущий символ
		char get_next_char();

		// возвращает текущий символ
		char & get_last_char();

		// делает шаг назад
		void step_back(int n);

		// проверка на готовность к работе
		bool ready();
	private:
		size_t position = 0; // текущая позиция 
		std::string str; // строка с json
	};

	// извлекает токены
	class tokenizer {
	public:
		// конструтор
		tokenizer(i_reader * _reader);

		// деструтор класса 
		~tokenizer();

		// возвращает следущий токен 
		token & get_next_token();

		// возвращает текущий токен
		token & get_last_token();
	private:

		// создает токен
		token create_token(token_type _t);
		token create_token(std::string str, token_type _t);
		token create_token(char ch, token_type _t);

		// получает значения между кавычками
		std::string string_parse();

		// получает число
		std::string number_parse();

		// парсит литералы
		bool literal_parse(std::string _literal);

		// возвращает тип символа
		token_type get_char_type(char & _char);

		token cur_token; // текущий токен
		i_reader * reader; // для чтения данных
	};

	// класс записи
	class i_writer {

	};

	// класс парсера
	class json_parser {
	public:
		// конструктор класса
		json_parser();

		// получение значения по ключу
		json_value get(const std::string & key);
		json_value get(const char * key);

		// проверка наличия ключа
		bool check(const std::string & key);
		bool check(const char * key);

		// загрузка из файла
		void load_from_file(const std::string & key);
		void load_from_file(const char * key);

		// загрузка из строки
		void load_from_string(const std::string & key);
		void load_from_string(const char * key);

	private:
		
		tokenizer * tokenizer; // для получения новых токенов
	};

};

#endif // ! __DRONJSON__
