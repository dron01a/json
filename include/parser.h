#ifndef _DRONJSON_PARSER_IMPL_
#define _DRONJSON_PARSER_IMPL_

#include "error.h"
#include "tokenizer.h"
#include "json_value.h"

namespace json {
	
	// класс конфигурации парсинга 
	class parse_config {
	public:
		// кодировки
		enum class encoding_mode : uint8_t { ascii, utf8 };

		// режимы синтаксиса
		enum class sinax_mode : uint8_t { STANDART = 0, JSON5, YALM };

		// режимы сбора ошибок
		enum class error_mode : uint8_t { strict, collect };

		// конструктор класса
		parse_config(encoding_mode enc = encoding_mode::ascii,
			sinax_mode sm = sinax_mode::STANDART,
			error_mode em = error_mode::strict);

		// возвращает текущюю кодировку
		encoding_mode & encoding();

		// возвращает текущий синтаксис
		sinax_mode & sinax();

		// возвращает текущий режим сбора ошибок
		error_mode & error_halding();

	private:
		encoding_mode _encoding;
		sinax_mode _sinax_mode;
		error_mode _error_mode;
	};

	parse_config standart();

	// ошибка парсинга
	class parse_error : public base_error {
	public:
		enum class error_code {
			_error_token,
			_invalid_value,
			_invalid_array_value,
			_invalid_object
		};

		parse_error(error_code code, size_t line, size_t col);
	private:
		std::string form_message(error_code code);
	};

	// результат выполенния парсинга
	struct parse_result {
		bool valid = false; // флаг удачного или нет результата
		json_value json_val; // значение json полученное в резульатате парсинга 
		std::vector<std::unique_ptr<base_error>> errors; // ошибки полученные при парсинге
	};

	namespace impl {

		using namespace io;
		using namespace io_base;
		using namespace encodings;

		// реализация dom-парсера
		class dom_parser_impl {
		public:

			// конструктор
			dom_parser_impl(i_input_ptr_ref input, parse_config & conf);
			
			// запуск парсинга
			parse_result parse();

			// установка режима сбора ошибок 
			void set_collect_mode(bool val);  ///???

		private:

			// парсинг json-начения
			json_value parse_json_value(std::vector<std::unique_ptr<base_error>> & errors);

			// парсинг массива 
			json_value parse_array(std::vector<std::unique_ptr<base_error>> & errors);

			// парсинг объекта
			json_value parse_object(std::vector<std::unique_ptr<base_error>> & errors);

			// собирает ошибки либо бросет ошибку
			void collect_or_throw(std::vector<std::unique_ptr<base_error>> & errors, parse_error::error_code code);

			bool _collect_mode = false;
			i_input_processor_ptr _input_proc; // входные данные
			i_decoder_ptr _decoder; // декодер 
			tokenizer_ptr _tokenizer; // токенайзер
		};

	} // impl

} // json

#endif // __DRONJSON_PARSER_IMPL_
