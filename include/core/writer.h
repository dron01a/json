#ifndef _DRONJSON_WRITER_
#define _DRONJSON_WRITER_

#include "error.h"
#include "io.h"
#include "json_value.h"

namespace json {

	enum class write_flags : uint32_t {
		format = 1 << 0, // выполняет форматирование
		using_tabs = 1 << 1, // заменяет все пробелы на табы
		single_quotes = 1 << 2, // одинарные ковычки
		write_bom = 1 << 3, // запись BOM

		/* добавить флаги на будущее 

		[] флаг компактных объектов 
		[] флаг компактных массивов
		[] все числа как hex

		*/
	};

	// конфигурация записи
	class write_config {
	public:

		// режимы сбора ошибок
		enum class error_mode : uint8_t { strict, collect };
		
		// конструктор класса
		write_config(encoding enc = encoding::ascii,
			output_format sm = output_format::JSON,
			error_mode em = error_mode::strict);

		// возвращает текущюю кодировку
		encoding & encoding();

		// возвращает текущий синтаксис
		output_format & sinax();

		// возвращает текущий режим сбора ошибок
		error_mode & error_halding();

		// устанавливает флаг
		void set_flag(write_flags flag);

		// сбрасывает флаги
		void reset_flag(write_flags flag);

		// проверяет наличе флага
		bool chesk_flag(write_flags flag);

		// устанавливает сивол отступа
		char & indent();

		// устанавливает символ пробела
		char & space();

		// устанавливает количество отступов
		size_t & indent_size();

		// устанавливает количество пробелов
		size_t & space_size();

	private:
		uint32_t _flags; // флаги
		json::encoding _encoding;
		output_format _sinax_mode;
		error_mode _error_mode;
		size_t _indent_size = 2; 
		size_t _space_size = 1;
		char _indent_char = ' ';
		char _space_char = ' ';
	};

	write_config format();

	// результат записи
	struct write_result {
		bool success = false; // успешная запись или нет
		std::vector<std::unique_ptr<error>> errors; // ошибки полученные при записи
	};

	namespace core {

		namespace impl {

			using namespace io;
			using namespace io::io_base;
			using namespace io::encodings;

			// класс записи
			class writer_impl {
			public:
				// конструктор 
				writer_impl(i_output_ptr_ref output, write_config conf);

				// записывает значение 
				write_result write(const json_value & json_val);

			private:

				// записывает json_value
				void write_value(const json_value & json_val, write_result & res);

				// запись объекта
				void write_object(const json_object & data, write_result & res);

				// запись массива
				void write_array(const json_array & data, write_result & res);

				i_output_ptr_ref _output; // куда выводить
				i_encoder_ptr _encoder; // энкодер
				i_output_processor_ptr _output_proc; // обработчик вывода 
			};

		}
	}
}

#endif