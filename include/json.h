#ifndef _DRONJSON_
#define _DRONJSON_

#include "error.h"
#include "json_value.h"
#include "parser.h"
#include "io_base.h"

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

}

#endif