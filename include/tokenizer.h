#ifndef _DRON_JSON_TOKENIZER_
#define _DRON_JSON_TOKENIZER_

#include "error.h"
#include "input_processor.h"

namespace json {

	namespace io {

		// клас токенайзера
		class tokenizer {
		public:
			// конструктор класса 
			tokenizer(i_input_processor_ptr_ref input_proc,
				      encodings::i_decoder_ptr_ref decoder,
				      size_t & line, 
				      size_t & col);

		    tokenizer(const tokenizer & tok);

			// деструктор класса 
			~tokenizer() {};
			
			// следующий токен 
			token & next();

			// последний полученный токен
			token & last();

		private:
			size_t & _line; // ссылка на линию ??
			size_t & _col;	// ссылка на столбец ??
			i_input_processor_ptr_ref _input_proc; // ссылка на обработчик ввода
			encodings::i_decoder_ptr_ref _decoder; // ссылка на декодер 
			token cur_token; // последний полученный токен
		};

	};

};

#endif // !_DRON_JSON_TOKENIZER_
