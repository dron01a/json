#ifndef __DRON_JSON_BASE_IO__
#define __DRON_JSON_BASE_IO__

#include <fstream>
#include <string>

#include "error.h"

namespace json {
	
	// базовый ввод вывод
	namespace io_base {

		// класс ошибки ввода-вывода
		class io_error : public base_error {
		public:

		private:

		};

		// интерфейс ввода данных
		class i_input {
		public:
			virtual ~i_input() = default;
			virtual std::char_traits<char>::char_type next_char() = 0;
			virtual std::char_traits<char>::char_type last_char() = 0;
			virtual void seek(int n) = 0;
			virtual bool ready() = 0;
			virtual bool eof() = 0;
		};

		// ввод из файла
		class file_input : public i_input {
		public:
			// конструктор
			explicit file_input(const std::string & file_name);
			explicit file_input(const char * file_name);

			// деструктор
			~file_input();

			// возвращает следующий символ
			std::char_traits<char>::char_type next_char();

			// возвращает предыдущий символ
			std::char_traits<char>::char_type last_char();

			// делает шаг назад
			void seek(int n);

			// проверка на готовность к работе
			bool ready();

			// конец файла
			bool eof();

		private:
			std::ifstream file;
			char cur_char;
		};

		// ввод из строки
		class string_input : public i_input {
		public:
			// конструктор
			explicit string_input(const std::string & string);
			explicit string_input(const char * string);

			// деструктор
			~string_input();

			// возвращает предыдущий символ
			std::char_traits<char>::char_type next_char();

			// возвращает текущий символ
			std::char_traits<char>::char_type last_char();

			// устанавливает позицию
			void seek(int n);

			// проверка на готовность к работе
			bool ready();

			// конец файла
			bool eof();

		private:
			size_t position = 0; // текущая позиция 
			std::string str; // строка с json
		};

		// ввод из потока
		class stream_input : public i_input {
		public:
			// конструктор
			explicit stream_input(std::istream & stream);

			// деструктор
			~stream_input();

			// возвращает предыдущий символ
			std::char_traits<char>::char_type next_char();

			// возвращает текущий символ
			std::char_traits<char>::char_type last_char();

			// делает шаг назад
			void seek(int n);

			// проверка на готовность к работе
			bool ready();

			// конец файла
			bool eof();

		private:
			std::istream * stream;
			char cur_char;
		};

		// интерфейс вывода данных
		class i_output {
		public:
			virtual ~i_output() = default;
			virtual void out_data(char data) = 0;
			virtual void out_data(const char * data) = 0;
			virtual bool ready() = 0;
		};

		// вывод из файла
		class file_output : public i_output {
		public:
			// конструтор класса
			explicit file_output(const std::string & file_name);
			explicit file_output(const char * file_name);

			~file_output();

			// запись данных
			void out_data(const char * data);
			void out_data(char data);

			// проверка готовности 
			bool ready();
		private:
			std::ofstream _desc; // файл для записи;
		};

		// вывод из строки
		class string_output : public i_output {
		public:
			// конструтор класса
			explicit string_output(std::string & _data);

			// запись данных
			void out_data(const char * data);
			void out_data(char data);

			// проверка готовности 
			bool ready();
		private:
			std::string * _desc; // указатель на строку куда записываем
		};

		// вывод из потока
		class stream_output : public i_output {
		public:
			// конструтор класса
			explicit stream_output(std::ostream & _data);

			~stream_output();

			// запись данных
			void out_data(const char * data);
			void out_data(char data);

			// проверка готовности 
			bool ready();
		private:
			std::ostream * _desc; // указатель на поток куда записываем
		};

	};
};

#endif // !__DRON_JSON_BASE_IO__
