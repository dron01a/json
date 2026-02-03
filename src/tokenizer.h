#ifndef _DRON_JSON_TOKENIZER_
#define _DRON_JSON_TOKENIZER_

#include <memory>

#include "error.h"
#include "input_processor.h"

namespace json {

	namespace io {

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
			union {
				double _double_data = 0;
				std::unique_ptr<std::string> _string_data;
			}; 
			token_type _type; // тип токена
		};

		// клас токенайзера
		class tokenizer {
		public:
			// конструктор класса 
			tokenizer(std::unique_ptr<json::io::i_input_processor> input_proc,
				      std::unique_ptr<json::io_base::i_input> input,
				      size_t & line, 
				      size_t & col);

			// деструктор класса 
			~tokenizer() = default;
			
			// следующий токен 
			token & next();

			// последний полученный токен
			token & last();

		private:

			// пропуск пробелов
			void skip_space();

			size_t * _line; // указатель на линию 
			size_t * _col;	// указатель на столбец 
			std::unique_ptr<json::io::i_input_processor> _input_proc;
			std::unique_ptr<json::io_base::i_input> _input;
			token cur_token; // последний полученный токен
		};

	};

};

#endif // !_DRON_JSON_TOKENIZER_
