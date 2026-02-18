#ifndef _DRONJSON_INPUT_PROCESSOR_
#define _DRONJSON_INPUT_PROCESSOR_

#include "error.h"
#include "io_base.h"
#include "encoding.h"

namespace json {

	namespace io {

		namespace fsm {
			
			// конечный автомат для пропуска коментариев и пробелов 
			class skip_coment_and_space_fsm {
			public:
				// конструтор класса
				skip_coment_and_space_fsm();
			
				// обработка
				void proc(encodings::i_decoder_ptr_ref decoder, size_t & line, size_t & col);

			private: 
				
				// состояния
				enum class state {
					_none = 0, // ничего
					_line_comment, // коммент в линию
					_block_comment, // блок коментариев
					_end // выход из автомата
				};

				// обработка состояния none
				void none_proc(encodings::i_decoder_ptr_ref decoder, size_t & line, size_t & col);

				// обработка состояния line_comment
				void line_comment_proc(encodings::i_decoder_ptr_ref decoder, size_t & line, size_t & col);

				// обработка состояния block_comment
				void block_comment_proc(encodings::i_decoder_ptr_ref decoder, size_t & line, size_t & col);

				state _state; // текущее состояние

			};

			// конечный автомат для обраотки чисел 
			class digit_parse_fsm {
			public:
				// конструтор класса
				digit_parse_fsm();
			
				// функция запуска автомата
				std::pair<double, bool> run(encodings::i_decoder_ptr_ref decoder, size_t & line, size_t & col);

			private:
				
				// возвращает тип симовола
				size_t char_type(char c);

				// оработка 

				// состояния
				enum class state {
					_none = 0, // ничего
					_sing, // + - 
					_digit, // собираем числа 
					_dot, // обработка ввыражения после точки
					_fract, // часть числа после точки
					_exp_s, // занк экспоненты 
					_exp, // экспонента 
					_error, // ошибка
			    	_end, // выход из автомата
				};

				// таблица перходов
				static constexpr state table[8][6] = { 
					//        + -		.			0 - 9				e E	   другие символы      , \0
	/* none */		{ state::_sing, state::_dot, state::_digit, state::_exp_s, state::_error, state::_end },
	/* sign */		{ state::_error, state::_dot, state::_digit, state::_exp_s, state::_error, state::_end },
	/* digit */		{ state::_error, state::_dot, state::_digit, state::_exp_s, state::_error, state::_end },
	/* dot */		{ state::_error, state::_error, state::_fract, state::_exp_s, state::_error, state::_end },
	/* fract */		{ state::_error, state::_error, state::_fract, state::_exp_s, state::_error, state::_end },
	/* exp_s */		{ state::_exp, state::_error, state::_exp, state::_error, state::_error, state::_end },
	/* exponent */	{ state::_error, state::_error, state::_exp, state::_error, state::_error, state::_end },
	/* error */		{ state::_end, state::_end, state::_end, state::_end, state::_end, state::_end },
				};

				state _state; // текущее состояние
			};

			class hex_parse_fsm {
			public:
				// конструтор класса
				hex_parse_fsm();

				// обработка
				std::pair<double, bool> run(encodings::i_decoder_ptr_ref decoder, size_t & line, size_t & col);
			private:

				// возвращает тип симовола
				size_t char_type(char c);

				// оработка 

				// состояния
				enum class state {
					_zero = 0, // начальная позиция 
					_prefix = 1, // x X
					_digit, // 0 - 9, a-f, A-F
					_error, // ошибка
					_end, // выход из автомата
				};

				// таблица перходов
				static constexpr state table[4][4] = {
					//		x X			0 - 9, a-f, A-F	  другие символы	     , \0
	/* zero */      { state::_prefix,	state::_error,	 state::_error,		state::_end },
	/* prefix */    { state::_error,	state::_digit,	 state::_error,		state::_end },
	/* digit */		{ state::_error,	state::_digit,	 state::_error,		state::_end },
	/* error */		{ state::_end,		state::_end,	 state::_end,		state::_end },
				};

				state _state; // текущее состояние
			};

		};

		// класс ошибки получаемой информации
		class input_error : public base_error {
		public:

			// коды ошибок
			enum class error_code {
				_error_token = 0,
				_invalid_string,
				_invalid_number,
				_invalis_hex_number,
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
			void string_data(std::string data);

			// возврат числовых данных 
			double double_data() const;
			void double_data(double data);

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

			virtual size_t line() = 0;

			virtual size_t col()  = 0;

		};

		// класс с базовым функционалом
		class base_input_processor : public i_input_processor {
		public:
			// конструтор класса
			base_input_processor();
			base_input_processor(const base_input_processor & bip);

			size_t line();
			size_t col();

		protected:

			// вспомогательные методы
			
			// обработка строки
			token parse_string(encodings::i_decoder_ptr_ref _decoder, char quote_char);
			
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

			fsm::digit_parse_fsm _dp_fsm; // конечный автомат для парсинга чисел

			size_t _line; // линия 
			size_t _col; // столбец

		};

		// класс обработки входа для JSON
		class json_input_processor : public base_input_processor {
		public:
			//конструктор класса
			json_input_processor();
			json_input_processor(const json_input_processor & jip);

			// получение токена
			token next_token(encodings::i_decoder_ptr_ref _decoder);

		private:

			fsm::skip_coment_and_space_fsm _scs_fsm; // конечный автомат для пропуска коментариев
		};

		// класс обработки входа json5 
		class json5_input_processor : public base_input_processor {
		public:
			//конструктор класса
			json5_input_processor();
			json5_input_processor(const json_input_processor & jip);

			// получение токена
			token next_token(encodings::i_decoder_ptr_ref _decoder);
		private:
			// обработка json5 чисел
			token parse_json5_number(encodings::i_decoder_ptr_ref _decoder);

			// обработка дестичного или шеснадцатиричного числа
			token parse_digit_or_hex(encodings::i_decoder_ptr_ref _decoder);

			// обработка hex-чисел
		//	token parse_hex_number(encodings::i_decoder_ptr_ref _decoder);

			// обработка бесконечности 
			token parse_infinity(encodings::i_decoder_ptr_ref _decoder, bool _negative);

			// обработка nan
			token parse_nan(encodings::i_decoder_ptr_ref _decoder, bool _negative);

			// обработка идендификатора или литерала
			token parse_literal_or_indentifier(encodings::i_decoder_ptr_ref _decoder);

			fsm::hex_parse_fsm _hex_fsm; // автомат для парсига hex-чисел
			fsm::skip_coment_and_space_fsm _scs_fsm; // конечный автомат для пропуска коментариев
		};

		// класс обработки входа yalm 
		class yalm_input_processor : public base_input_processor {
		public:
			//конструктор класса
			yalm_input_processor();
			yalm_input_processor(const json_input_processor & jip);

			// получение токена
			token next_token(encodings::i_decoder_ptr_ref _decoder);
		};

		using i_input_processor_ptr = std::unique_ptr<i_input_processor>;
		using i_input_processor_ptr_ref = std::unique_ptr<i_input_processor>&;

	}; // io

}; // json

#endif // !_DRONJSON_INPUT_PROCESSOR_
