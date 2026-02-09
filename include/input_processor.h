#ifndef _DRONJSON_INPUT_PROCESSOR_
#define _DRONJSON_INPUT_PROCESSOR_

#include "error.h"
#include "io_base.h"
#include "encoding.h"

namespace json {

	namespace io {

		// класс ошибки получаемой информации
		class input_error : public base_error {
		public:

			// коды ошибок
			enum class error_code {
				_error_token = 0,
				_invalid_string,
				_invalid_number,
				_invalid_number_format,
				_invalid_escape,
				_invalid_unicode_char,
				_invalid_unicode_low_pair,
				_literal_error,
			};

			input_error(error_code code, size_t & line, size_t & col);

		private:
			std::string form_message(error_code code);
		};

		// типы токенов
		enum class token_type {
			_none = 0,
			_space, // в том числе и табуляции
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

		// класс токена
		class token {
		public:
			// конструтор класса
			token();
			token(const token & other);
			token(token_type t);
			token(double data);
			token(const char * data);
			token(const std::string & data);

			// деструктор
			~token();

			token & operator=(const token & other);

			// возврат строковых данных 
			std::string string_data() const;

			// возврат числовых данных 
			double double_data() const;

			token_type type() const;
			void type(token_type t);
		private:

			// копирование данных
			void copy_data(const token & other);

			//union для хранения данных 
			union data {
				double _double_data;
				std::string _string_data;
				data() : _double_data(0) {}
				~data() {};
			} _data;
			token_type _type; // тип токена
		};

		// интерфейс для обработчика входа
		class i_input_processor {
		public:
			virtual ~i_input_processor() = default;
			
			// получение токена
			virtual token next_token(encodings::i_decoder_ptr_ref _decoder) = 0;

		};

		// класс с базовым функционалом
		class base_input_processor : public i_input_processor {
		public:
			// конструтор класса
			base_input_processor(size_t& line, size_t& col);
			base_input_processor(const base_input_processor & bip);

		protected:

			// вспомогательные методы
			
			// обработка строки
			token parse_string(encodings::i_decoder_ptr_ref _decoder);
			
			// обработка escape последовательности
			std::string parse_escape(encodings::i_decoder_ptr_ref _decoder);

			// обработка unicode последовательности
			std::string parse_unicode(encodings::i_decoder_ptr_ref _decoder);

			// обработка unicode пары
			uint32_t parse_unicode_pair(encodings::i_decoder_ptr_ref _decoder);

			// обработка числа 
			token parse_number(encodings::i_decoder_ptr_ref _decoder);

			// обработка числа 
			token parse_literal(encodings::i_decoder_ptr_ref _decoder, const char * literal_str, token_type type);

			// пропуск пробелов и табуляций
			void skip_space(encodings::i_decoder_ptr_ref _decoder);

			// пропуск коментариев
			void skip_coments(encodings::i_decoder_ptr_ref _decoder);

			// ссылки на линию и столбец
			size_t & _line;
			size_t & _col;

		};

		// класс обработки входа для JSON
		class json_input_processor : public base_input_processor {
		public:
			//конструктор класса
			json_input_processor(size_t& line, size_t& col);
			json_input_processor(const json_input_processor & jip);

			// получение токена
			token next_token(encodings::i_decoder_ptr_ref _decoder);
		};

		using i_input_processor_ptr = std::unique_ptr<i_input_processor>;
		using i_input_processor_ptr_ref = std::unique_ptr<i_input_processor>&;

	}; // io

}; // json

#endif // !_DRONJSON_INPUT_PROCESSOR_
