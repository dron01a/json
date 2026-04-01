#ifndef _DRONJSON_ENCODING_
#define _DRONJSON_ENCODING_

#include <vector>

#include "io_base.h"

namespace json {

	enum class encoding {
		ascii,
		utf8,
	};

	namespace core {
	
		namespace io {
		
			namespace encodings {

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

					// возвращает тип кодировки
					virtual encoding type() = 0;
				};

				// базовый декодер 
				class base_decoder : public i_decoder {
				public:
					explicit base_decoder(io_base::input_ref input);
					virtual char32_t next_char() = 0;
					char32_t current_char();
					char32_t peek_char();
					virtual void position(size_t pos) = 0;
					size_t position();
					void clear_peek_buff();
					void push_buff(char32_t c);
					bool eof();
					encoding type();
				protected:
					char32_t _cur_char; // текущий сивол
					std::vector<char32_t> _buff; // буфер для символов 
					size_t _position; // позиция
					bool _eof = false; // конец
					io_base::input & _input; // источник получения данных
					encoding _type;
				};

				// декодер utf8
				class utf8_decoder : public base_decoder {
				public:
					explicit utf8_decoder(io_base::input_ref input);
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
					explicit ascii_decoder(io_base::input_ref input);
					char32_t next_char();
					void position(size_t pos);
				};

				// интерфейс энкодера
				class i_encoder {
				public:
					virtual ~i_encoder() = default;
					virtual void encode_string(const std::string & string) = 0;
					virtual void encode_code(char32_t code) = 0;
					virtual void add_bom() = 0;
					virtual io_base::i_output_ptr_ref output() = 0;
				};

				// знкодер utf8
				class utf8_encoder : public i_encoder {
				public:
					utf8_encoder(io_base::i_output_ptr_ref dest);
					void encode_code(char32_t code);
					void encode_string(const std::string & string);
					void add_bom();
					io_base::i_output_ptr_ref output();
				private:
					io_base::i_output_ptr_ref _output; // куда записываем данные
				};

				// знкодер ascii
				class ascii_encoder : public i_encoder {
				public:
					ascii_encoder(io_base::i_output_ptr_ref dest);
					void encode_code(char32_t code);
					void encode_string(const std::string & string);
					void add_bom() {};
					io_base::i_output_ptr_ref output();
				private:
					io_base::i_output_ptr_ref _output; // куда записываем данные
				};

				using i_decoder_ptr = std::unique_ptr<i_decoder>;
				using i_decoder_ptr_ref = std::unique_ptr<i_decoder> &;
				using i_encoder_ptr = std::unique_ptr<i_encoder>;
				using i_encoder_ptr_ref = std::unique_ptr<i_encoder> &;

			} // encodings

		} // io

	} // core 

} // json

#endif // !_DRONJSON_ENCODING_
