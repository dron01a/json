#ifndef __DRONJSON__READER__
#define __DRONJSON__READER__

#include <fstream>

#include "error.h"

namespace json {

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
	class file_reader : public i_reader {
	public:

		// конструктор
		explicit file_reader(const std::string & file_name);
		explicit file_reader(const char * file_name);

		// деструктор
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

		// деструктор
		~string_reader() {};

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

		// деструктор
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

};

#endif