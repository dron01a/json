#ifndef _DRONJSON_BASE_IO_
#define _DRONJSON_BASE_IO_

#include <fstream>
#include <string>
#include <memory>

#include "error.h"

#define INPUT_BUFFER_SIZE 8192

namespace json {
	
	namespace core {

		namespace io {

			// базовый ввод вывод
			namespace io_base {

				using char_traits = std::char_traits<char>;
				using int_type = char_traits::int_type;
				
				// класс ошибки ввода-вывода
				class io_error : public error {
				public:
					enum class error_code {
						_file_not_found,
						_string_is_empty,
						_stream_is_bad,
						_invalid_file,
						_invalid_string,
						_invalid_stream,
					};

					io_error(error_code code, size_t line, size_t col, std::string file_name = "");

				private:
					std::string form_message(error_code code, std::string file_name);
				};

				// возвращает значение окончания файла
				int_type eof_char();

				enum class dir {
					 _begin,
					 _cur,
					 _end
				};

				class input_policy {
				public:
					virtual ~input_policy() = default;
					virtual size_t fill_buff(char *, size_t) = 0;
					virtual bool eof() = 0;
					virtual void seekg(int pos, dir dir = dir::_cur) = 0;
					virtual size_t pos() = 0;
					virtual bool good() = 0; 
				};

				class string_input_policy : public input_policy {
				public:

					// конструтор класса
					explicit string_input_policy(const std::string & src);

					// заполнение буфера
					size_t fill_buff(char * buff, size_t buff_size) override;

					// возвращает true если достигнут конец
					bool eof() override;

					// перемещение 
					void seekg(int pos, dir dir = dir::_cur) override;

					// возвращает текущюю позицию 
					size_t pos();

					// возвращает true если готов к работе
					bool good(); 

				private:

					int _pos; // текущая позиция 
					std::string _src; // строка с информацией 
				};

				class file_input_policy : public input_policy {
				public:

					// конструтор класса
					explicit file_input_policy(const std::string & file_name);

					// деструктор 
					~file_input_policy();

					// заполнение буфера
					size_t fill_buff(char * buff, size_t buff_size) override;

					// возвращает true если достигнут конец
					bool eof() override;

					// перемещение 
					void seekg(int pos, dir dir = dir::_cur) override;

					// возвращает текущюю позицию 
					size_t pos() override;

					// возвращает true если готов к работе
					bool good();

				private:

					std::ifstream _file; // файловый поток
				};
				
				class stream_input_policy : public input_policy {
				public:
					// конструтор класса
					explicit stream_input_policy(std::istream & stream);

					// деструктор 
					~stream_input_policy();

					// заполнение буфера
					size_t fill_buff(char * buff, size_t buff_size) override;

					// возвращает true если достигнут конец
					bool eof() override;

					// перемещение 
					void seekg(int pos, dir dir = dir::_cur) override;

					// возвращает текущюю позицию 
					size_t pos() override;

					// возвращает true если готов к работе
					bool good();

				private:
					std::istream * _stream;
				};

				// класс ввода даннных
				class input {
				public:

					// конструктор класса
					input(std::unique_ptr<input_policy> policy);

					// следующий символ
					int_type next_char();

					// предыдущий символ
					int_type last_char();

					// перемещение
					void seekg(int pos, dir dir = dir::_cur);

					// проверка состояния 
					bool good();

					// проверка конца
					bool eof();

				private:

					char _buff[INPUT_BUFFER_SIZE]; // буфер для хранения данных
					size_t _buff_pos; // текущая позиция в буфере
					size_t _buff_size; // текущий размер буфера
					std::unique_ptr<input_policy> _policy; // политика ввода данных
				};

				// интерфейс вывода данных
				class i_output {
				public:
					virtual ~i_output() = default;
					virtual void out_data(char data) = 0;
					virtual void out_data(const char * data) = 0;
					virtual void out_data(char32_t data) = 0;
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
					void out_data(char32_t data);

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
					void out_data(char32_t data);

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
					void out_data(char32_t data);

					// проверка готовности 
					bool ready();
				private:
					std::ostream * _desc; // указатель на поток куда записываем
				};

				using input_ref = input &;
				using i_output_ptr = std::unique_ptr<i_output>;
				using i_output_ptr_ref = std::unique_ptr<i_output> &;
			};

		}
	}
};

#endif // !_DRONJSON_BASE_IO_
