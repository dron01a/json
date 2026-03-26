#ifndef _DRONJSON_IO_
#define _DRONJSON_IO_

#include "tokenizer.h"
#include "input_processor.h"
#include "output_processor.h"

namespace json {
	
	enum class json_sinax {
		JSON = 0, 
		JSON5
	};

	enum class output_format {
		JSON = 0,
		XML
	};

	namespace core {

		namespace io {

			// возвращает декодер в зависимости от типа кодировки
			encodings::i_decoder_ptr make_decoder(encoding enc, io_base::i_input_ptr_ref input);

			// возвращает энкодер в зависимости от типа кодировки
			encodings::i_encoder_ptr make_encoder(encoding enc, io_base::i_output_ptr_ref output);

			// возвращает обработчик ввода в зависимости от типа синтаксиса json 
			i_input_processor_ptr make_input_processor(json_sinax sinax);

			// возвращает обработчик вывода в зависимости от типа формата 
			i_output_processor_ptr make_output_processor(output_format format, encodings::i_encoder_ptr_ref dest, bool format_flag);

		}
	
	}

}


#endif