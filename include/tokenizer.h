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
				      std::vector<std::unique_ptr<base_error>> & errors);

		    tokenizer(const tokenizer & tok);

			// деструктор класса 
			~tokenizer() {};
			
			// следующий токен 
			token & next();

			// последний полученный токен
			token & last();

		private:
			i_input_processor_ptr_ref _input_proc; // ссылка на обработчик ввода
			encodings::i_decoder_ptr_ref _decoder; // ссылка на декодер 
			std::vector<std::unique_ptr<base_error>> & _errors; // ссылка на вектор с ошибками 

			token cur_token; // последний полученный токен
		};

		using tokenizer_ptr = std::unique_ptr<json::io::tokenizer>;
	};

};

#endif // !_DRON_JSON_TOKENIZER_
