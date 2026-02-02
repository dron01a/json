#ifndef _DRONJSON_INPUT_PROCESSOR_
#define _DRONJSON_INPUT_PROCESSOR_

#include "error.h"
#include "io_base.h"
#include "encoding.h"

namespace json {

	namespace io {

		// базовые обработчики
		namespace basic_processors {

			// строковый обработчик
			struct string_proc{
				std::string operator()(json::io_base::i_input * src, json::encodings::i_decoder * decoder, size_t & line, size_t & col);
			};

			// числовой обработчик
			struct digit_proc {
				double operator()(json::io_base::i_input * _src, size_t & col);
			};

			// обработчик литералов
			struct literal_proc {
				bool operator()(json::io_base::i_input * _src, size_t & col, std::string liter);
			};

		};

		// интерфейс для обработчика входа
		class i_input_processor {
		public:
			virtual ~i_input_processor() = default;
			
			// обработка строки 
			virtual std::string string_processing() = 0;

			// обработка строки 
			virtual double digit_processing() = 0;

			// обработка литералов
			virtual bool literal_processing(std::string liter) = 0;

		};

		// класс обработки входной информации 
		template <typename string_processor, 
				  typename digit_processor, 
				  typename literal_processor>
		class input_processor : public i_input_processor {
		private:
			json::io_base::i_input * _src;
			json::encodings::i_decoder * _decoder;
			size_t * _line;
			size_t * _col;
			string_processor sp; 
			digit_processor dp;
			literal_processor lp;
		public:
			// конструктор класса
			explicit input_processor(json::io_base::i_input * src, json::encodings::i_decoder * decoder, size_t & line, size_t & col){
				_line =& line;
				_col =& col;
				_src = src;
				_decoder = decoder;
			}

			~input_processor() {}

			// обработка строки 
			std::string string_processing() {
				return sp(_src, _decoder, *_line, *_col);
			}

			// обработка строки 
			double digit_processing() {
				return dp(_src, *_col);
			}
		
			// обработка литералов
			bool literal_processing(std::string liter) {
				return lp(_src, *_col, liter);
			}
		};

		using simple_input_processor = input_processor<basic_processors::string_proc,
												       basic_processors::digit_proc,
													   basic_processors::literal_proc>;

	};

};

#endif // !_DRONJSON_INPUT_PROCESSOR_
