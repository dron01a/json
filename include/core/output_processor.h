#ifndef _DRONJSON_OUTPUT_PROCESSOR_
#define _DRONJSON_OUTPUT_PROCESSOR_

#include "error.h"
#include "io_base.h"
#include "encoding.h"

#include <stack>

namespace json {

	namespace io {

		// интерфейс класса вывода инфомации
		class i_output_processor {
		public:

			// конструктор 
			i_output_processor(encodings::i_encoder_ptr_ref dest, bool format = false);

			// деструктор  
			virtual ~i_output_processor() = default;

			// запись bom
			virtual void write_bom() = 0;
			
			// запись null
			virtual void write_null() = 0;

			// запись чисел типа int
			virtual void write_int(int data) = 0;
			
			// запись чисел типа unsigned int 
			virtual void write_uint(unsigned int data) = 0;
			
			// запись чисел типа double
			virtual void write_double(double data) = 0;

			// запись bool 
			virtual void write_bool(bool data) = 0;

			// запись строки 
			virtual void write_string(const char * data) = 0;

			// начало массива
			virtual void begin_array() = 0;

			// конец массива 
			virtual void end_array() = 0;

			// начало объекта
			virtual void begin_object() = 0;

			// конец объекта
			virtual void end_object() = 0;

			// запись ключа
			virtual void write_key(const char * key) = 0;

			// пустой символ 
			void write_indent();

			// пробел
			void write_space();

			// переход на новую линию
			void write_new_line();
			
			// устанавливает сивол отступа
			void set_indent(char _char);

			// устанавливает символ пробела
			void set_space(char _char);

			// устанавливает колличество сиволов отступа
			void set_indent_size(size_t count);

			// устанавливает колличество сиволов пробела
			void set_space_size(size_t count);

			// возвращает текущий encoder
			encodings::i_encoder_ptr_ref encoder();

		protected:

			// экранирование строк
			void escape_string(const char * data);

			char _indent_char; // символ прин€тый в качестве отступа при форматировнии
			char _space_char; // символ прин€тый в качестве пробела
			size_t _indent_size; // колличество символов отступа
			size_t _space_size; // колличество символов пробела
			size_t _indent_level; // уровень вложенности элемента
			bool _format; // флаг форматировани€ вывода
			bool _need_comma; // нужна или не нужна зап€та€ 
			bool _after_key;
			encodings::i_encoder_ptr_ref _encoder; // энкодер
		};

		// стандартный вывод json
		class json_output_processor : public i_output_processor {
		public:
			// конструктор 
			explicit json_output_processor(encodings::i_encoder_ptr_ref dest, bool format = false);
			void write_bom() override;
			void write_null() override;
			void write_int(int data) override;
			void write_uint(unsigned int data) override;
			void write_double(double data) override;
			void write_bool(bool data) override;
			void write_string(const char * data) override;
			void begin_array() override; 
			void end_array() override;
			void begin_object() override;
			void end_object() override;
			void write_key(const char * key) override;
		
		private:

			// проверка перед вставкой элемента 
			void before_value();

			// проверка после вставки элемента 
			void end_value();

			// пишет зап€тую
			void write_comma();
			
			// пишет двоеточие
			void write_colon();

		};

		// выодит json как xml
		class xml_output_processor : public i_output_processor {
		public:
			// конструктор 
			explicit xml_output_processor(encodings::i_encoder_ptr_ref dest, bool format = false);
			explicit xml_output_processor(encodings::i_encoder_ptr_ref dest, std::string & root_name, bool format = false);
			void write_bom() override;
			void write_null() override;
			void write_int(int data) override;
			void write_uint(unsigned int data) override;
			void write_double(double data) override;
			void write_bool(bool data) override;
			void write_string(const char * data) override;
			void begin_array() override;
			void end_array() override;
			void begin_object() override;
			void end_object() override;
			void write_key(const char * key) override;

		private :
			
			// передварительна€ подготовка перед записью значени€ 
			void before_value();

			// записывает закрывающий тег
			void write_close_tag();

			std::stack<std::string> _tag_names; // имена тегов дл€ записи закрвающих тегов
			bool _in_array_flag; // флаг дл€ навигации (true, если находимс€ в массиве)
			size_t _arr_level; // дл€ записи вложенных массивов  
			size_t _obj_level; // дл€ записи вложенности объектов
			
		};

		using i_output_processor_ptr = std::unique_ptr<i_output_processor>;
		using i_output_processor_ptr_ref = std::unique_ptr<i_output_processor>&;

	}; // io

}; // json

#endif // !_DRONJSON_OUTPUT_PROCESSOR_
