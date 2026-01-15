#ifndef __DRONJSON__
#define __DRONJSON__

#include <cctype>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <fstream>
#include <stdio.h>

namespace json {

	class i_reader;
	class i_writer;
	class json_value;

	using json_object = std::map<std::string, json_value>;
	using json_array = std::vector<json_value>;

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
		token() { num_data = 0; };
		token(token_type _t) : _type(_t) { num_data = 0; };
		token(const token & t) {
			_type = t._type;
			switch (t._type){
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

	// структура ошибки
	struct error {
		error() {}
		error(size_t _c, size_t _s, error_type _e) : col(_c), str(_s), type(_e) {}
		size_t col = 0, str = 0;
		error_type type = error_type::_none;
	};

	// класс обработки строк с учетом escape-последовательностей и юникода
	class string_coder {
	public:
		// конструктор класса
		string_coder() {};

		// декодирование
		std::string decode(i_reader * reader, size_t & line, size_t & col);

		// кодирование
		std::string encode(const std::string & src_string);
	private:

		// чтение значений юникода 
		char32_t read_unicode(i_reader * reader, size_t & line, size_t & col);

		// чтение меньшей пары
		char32_t read_low_pair(i_reader * reader, size_t & line, size_t & col);

		// преобразование utf32 в utf8
		std::string utf32_to_utf8(char32_t code);

		// экранирует сиволы unicode
		std::string unicode_escape(uint32_t code);

		// оределяет длинну utf8 
		size_t get_utf8_len(unsigned char fb);

		// декодирование utf8 
		uint32_t decode_utf8(const std::string & str, size_t & position);
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
		json_value(const char * name, const json_value & val);
		json_value(const char * name, const json_value && val);

		json_value & operator=(const json_value & jval);
		json_value & operator=(json_value && jval);

		// деструктор класса
		~json_value();

		// для получения булевого значения 
		bool & as_bool();
		bool & as_bool() const;
		void as_bool(bool & val);

		// для получения числового значения
		double & as_num();
		double & as_num() const;
		void as_num(double num);

		// для получения строки
		std::string & as_string();
		std::string & as_string() const;
		void as_string(const std::string & string);
		void as_string(const char * string);

		// для получения массива
		json_array * as_array();
		json_array * as_array() const;
		void as_array(json_array array);

		// для получения объекта
		json_object * as_object();
		json_object * as_object() const;
		void as_object(json_object object);

		// возвращает занчение по имени
		json_value * find(const std::string & name);
		json_value * find(const char * name);

		// добавляет элемент в json (при условии что масиив)
		json_value * add(const json_value & val);
		json_value * add(json_value && val);

		// добавляет элемент в json (при условии что объект)
		json_value * add(const char * name, const json_value & val);
		json_value * add(const char * name, json_value && val);
		json_value * add(const std::string & name, const json_value & val);
		json_value * add(const std::string & name, json_value && val);

		// возвращает тип
		value_type type() const;
		void type(value_type _t);

	private:

		// очистка данных в зависимости от типа
		void clear_data();

		// перемещение данных 
		void copy_data(const json_value & val);

		// копирование данных
		void move_data(json_value && val);

		// поиск заначения в массиве 
		json_value * find_in_array(const char * name);

		// поиск значения в объекте
		json_value * find_in_object(const char * name);

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

	// чтение из потока
	class stream_reader : public i_reader {
	public:
		// конструктор
		explicit stream_reader(std::istream & stream);

		// деструтор
		~stream_reader();

		// возвращает предыдущий символ
		char get_next_char();

		// возвращает текущий символ
		char & get_last_char();

		// делает шаг назад
		void step_back(int n);

		// проверка на готовность к работе
		bool ready();
	private:
		std::istream * stream;
		char cur_char; 
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

	// интерфейс класса записи 
	class i_writer {
	public:
		virtual ~i_writer() = default;
		virtual void write_data(const char * data) = 0;
		virtual bool ready() = 0;
	};

	class string_writer : public i_writer {
	public:
		// конструтор класса
		explicit string_writer(std::string & _data);

		// запись данных
		void write_data(const char * data);

		// проверка готовности 
		bool ready();
	private:
		std::string * _desc; // указатель на строку куда записываем
	};

	class file_writer : public i_writer {
	public:
		// конструтор класса
		explicit file_writer(const std::string & file_name);
		explicit file_writer(const char * file_name);

		~file_writer();

		// запись данных
		void write_data(const char * data);

		// проверка готовности 
		bool ready();
	private:
		std::ofstream _desc; // файл для записи;
	};

	class stream_writer : public i_writer {
	public:
		// конструтор класса
		explicit stream_writer(std::ostream & _data);

		~stream_writer();

		// запись данных
		void write_data(const char * data);

		// проверка готовности 
		bool ready();
	private:
		std::ostream * _desc; // указатель на поток куда записываем
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

	private:

		// запускает парсинг
		json_value parse();

		// парсинг json-начения
		json_value parse_json_value(); 

		// парсинг массива 
		json_value parse_array();

		// парсинг объекта
		json_value parse_object();

		tokenizer * _tokenizer; // для получения новых токенов

		error last_error; // последняя полученная ошибка
	};

	// класс для записи json
	class json_writer{
	public:
		// конструктор класса
		json_writer();

		// деструктор класса
		~json_writer();

		// записывает в файл
		void write_to_file(const json_value & json_val, const std::string & file_name);
		void write_to_file(const json_value & json_val, const char * file_name);

		// записывает в строку
		void write_to_string(const json_value & json_val, std::string & json_string);

		// записывает в поток
		void write_to_stream(const json_value & json_val, std::ostream & stream);

	private:
		// записывает json_value 
		void write(const json_value & json_val);

		// записывает булево значение
		void write_bool(const bool & data);

		// записывает число
		void write_digit(const double & data);

		// записывает строку
		void write_string(const std::string & data);
		
		// записвает массив
		void write_array(const json_array & data);

		// записывает объект 
		void write_object(const json_object & data);

		i_writer * writer; // указатель на записывающий класс
	};


};

#endif // ! __DRONJSON__
