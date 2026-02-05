#ifndef __DRON_JSON_ENCODING__
#define __DRON_JSON_ENCODING__

#include <memory>
#include <vector>

#include "io_base.h"

namespace json {

	namespace encodings {

		class encoding_error : public base_error {
		public:
			enum class error_code { // коды ошибок
				_invalid_string,
				_invalid_escape,
				_invalid_unicode_char,
				_invalid_unicode_low_pair,
			};

			// конструктор 
			encoding_error(error_code code, size_t line, size_t col);
		private:

			// формирование сообщения
			std::string form_message(error_code code);

			error_code code;
		};

		// интерфейс декодера
		class i_decoder {
		public:
			virtual ~i_decoder() = default;

			// получение следующего символа
			virtual char32_t next_char() = 0;

			// получение текущего символа
			virtual char32_t current_char() = 0;

			// посмотреть символ впереди
			virtual char32_t peek_char() = 0;

			// осчистка буфера 
			virtual void clear_peek_buff() = 0;

			// устанавливает и возвращает позицию
			virtual void position(size_t pos) = 0;
			virtual size_t position() = 0;

			// отправляет символ в буфер
			virtual void push_buff(char32_t c) = 0;

			// возвращает флаг конца 
			virtual bool eof() = 0;
		};

		// интерфейс энкодера
		class i_encoder {
		public:
			virtual ~i_encoder() = default;
			virtual void encode(json::io_base::i_output * _dest, const std::string & _string) = 0;
		};

		// базовый декодер 
		class base_decoder : public i_decoder {
		public:
			explicit base_decoder(std::unique_ptr<io_base::i_input> input);
			virtual char32_t next_char() = 0;
			char32_t current_char();
			char32_t peek_char();
			virtual void position(size_t pos) = 0;
			size_t position();
			void clear_peek_buff();
			void push_buff(char32_t c);
			bool eof();
		protected:
			char32_t _cur_char; // текущий сивол
			std::vector<char32_t> _buff; // буфер для символов 
			size_t _position; // позиция
			bool _eof = false; // конец
			std::unique_ptr<io_base::i_input> _input; // источник получения данных
		};

		// декодер utf8
		class utf8_decoder : public base_decoder {
		public:
			explicit utf8_decoder(std::unique_ptr<io_base::i_input> input);
			char32_t next_char();
			void position(size_t pos);
		private:
			char32_t read_impl();
			bool skip_bom();
			bool _bom = false; // наличие BOM

		};

		// декодер ascii
		class ascii_decoder : public base_decoder {
		public:
			explicit ascii_decoder(std::unique_ptr<io_base::i_input> input);
			char32_t next_char();
			void position(size_t pos);
		};

	}

};

#endif // !__DRON_JSON_ENCODING__
