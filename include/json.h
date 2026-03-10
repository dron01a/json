#ifndef _DRONJSON_
#define _DRONJSON_

#include "error.h"
#include "json_value.h"
#include "parser.h"
#include "io_base.h"
#include "writer.h"

namespace json {

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
		static void parse(json::parse_result & res, json::io_base::i_input_ptr_ref input, parse_config & config);
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

}

#endif