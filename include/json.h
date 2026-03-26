#ifndef _DRONJSON_
#define _DRONJSON_

#include "core\error.h"
#include "core\json_value.h"
#include "core\parser.h"
#include "core\io_base.h"
#include "core\writer.h"

namespace json {

	using namespace json;
	using namespace json::core::io;
	using namespace json::core::io::io_base;
	using namespace json::core::io::encodings;
	using namespace json::core::impl;

	// класс DOM-парсера
	class dom_parser {
	public:

		// запускает парсинг из файла 
		static parse_result from_file(const char * file_name, parse_config config = standart());
		static parse_result from_file(const std::string & file_name, parse_config config = standart());
		
		// запускает парсинг из потока
		static parse_result from_stream(std::istream & stream, parse_config config = standart());

		// запускает парсинг из строки
		static parse_result from_string(const char * str, parse_config config = standart());
		static parse_result from_string(const std::string & str, parse_config config = standart());
	
	private:

		// запускает парсинг
		static void parse(json::parse_result & res, i_input_ptr_ref input, parse_config & config);
	};

	// класс для записи данных 
	class writer {
	public:
		// запись в файл 
		static write_result to_file(const json_value & val, const char * file_name, write_config config = format());

		// запись в поток 
		static write_result to_stream(const json_value & val, std::ostream & stream, write_config config = format());

		// запись в строку
		static write_result to_string(const json_value & val, std::string & dest, write_config config = format());

	};

	// интерфейс для конвертации в xml
	class xml_convert {
	public:
		// запись в файл 
		static void to_file(const json_value & val, const char * file_name, write_config config);

		// запись в поток 
		static void to_stream(const json_value & val, std::ostream & stream, write_config config);

		// запись в строку
		static std::string to_string(const json_value & val, write_config config);
	
	private:

		// основная функция для конвертации
		static void convert(io_base::i_output_ptr_ref out, const json_value & val, write_config config);
	};

}

#endif