#ifndef __DRONJSON__WRITER__
#define __DRONJSON__WRITER__

#include <fstream>

#include "json_reader.h"
#include "json_value.h"

namespace json{
	
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
	
	enum class write_flags : uint32_t {
		format = 1 << 0, // выполняет форматирование документа
		using_tabs = 1 << 1, // заменяет все пробелы на табы
		single_quotes = 1 << 2, // одинарные ковычки
		trailing_commas = 1 << 3, // всегда добавлять запятые в конце
		ascii_only = 1 << 4, // использовать только ASCII сиволы
		write_bom = 1 << 5, // запись BOM
		bool_as_num = 1 << 6, // записывает тип bool как 1 или 0
	};
	
	// класс конфигурации для записывателя
	class write_config {
	public:
		// конструктор класса
		write_config();
	
		// устанавливает флаг
		void set_flag(write_flags flag);
	
		// сбрасывает флаг 
		void reset_flag(write_flags flag);
	
		// сбрасывает флаги 
		void reset();
	
		// проверяет флаг на наличие
		bool has_flag(write_flags flag);
	
		// символ используемый в качестве пустого символа
		void indent(const char * str);
		std::string & indent();
		std::string indent() const;
	
		// устанавливает сивол пробела
		void space(const char * str);
		std::string & space();
		std::string space() const;
	
		// устанавливает колличество пробелов
		void space_count(size_t n);
		size_t & space_count();
		size_t space_count() const;
	
		// устанавливает новую линию
		void new_line(const char * str);
		std::string & new_line();
		std::string new_line() const;
	
		// устанавливает колличество числе после запятой
		void presition(size_t n);
		size_t & presition();
		size_t presition() const;
	
	private:
		uint32_t _flags; // флаги
		std::string _indent = " "; // пустай символ
		std::string _new_line = "\n"; // новая линия
		std::string _space = " "; // пробел 
		size_t _space_count = 2; // колличество пробелов
		size_t _presit = 15; // колличество цифр в числе с плавающей точкой после запятой
	};
	
	/*
		компактный конфиг
		записывает все в одну строку
		минимум пробелов
	*/
	write_config compact();
	
	/*
		конфиг с форматированием
		присутствуют все необходимые пробелы и переносы строк
	*/
	write_config format();
	
	// класс для записи json (записыватель)
	class json_writer {
	public:
		// конструктор класса
		json_writer(write_config config = compact());
	
		// записывает в файл
		void write_to_file(const json_value & json_val, const std::string & file_name);
		void write_to_file(const json_value & json_val, const char * file_name);
	
		// записывает в строку
		void write_to_string(const json_value & json_val, std::string & json_string);
	
		// записывает в поток
		void write_to_stream(const json_value & json_val, std::ostream & stream);
	
		// конфигурация 
		void config(write_config conf);
		write_config & config();
		write_config config() const;
	
	private:
		// записывает пустые символы
		void write_indent();
	
		// записывает пробелы
		void write_space();
	
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
		write_config _config; // конфигурация записывания
		size_t indent_level = 0;
	};

}

#endif