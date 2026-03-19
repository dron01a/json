#ifndef _DRONJSON_PARSER_IMPL_
#define _DRONJSON_PARSER_IMPL_

#include <stack>

#include "error.h"
#include "tokenizer.h"
#include "json_value.h"

namespace json {
	
	// класс конфигурации парсинга 
	class parse_config {
	public:
		// кодировки
		enum class encoding_mode : uint8_t { ascii, utf8 };

		// режимы синтаксиса
		enum class sinax_mode : uint8_t { STANDART = 0, JSON5, YALM };

		// режимы сбора ошибок
		enum class error_mode : uint8_t { strict, collect };

		// конструктор класса
		parse_config(encoding_mode enc = encoding_mode::ascii,
			sinax_mode sm = sinax_mode::STANDART,
			error_mode em = error_mode::strict);

		// возвращает текущюю кодировку
		encoding_mode & encoding();

		// возвращает текущий синтаксис
		sinax_mode & sinax();

		// возвращает текущий режим сбора ошибок
		error_mode & error_halding();

	private:
		encoding_mode _encoding;
		sinax_mode _sinax_mode;
		error_mode _error_mode;
	};

	parse_config standart();

	// ошибка парсинга
	class parse_error : public base_error {
	public:
		enum class error_code {
			_error_token,
			_invalid_value,
			_invalid_array_value,
			_invalid_object
		};

		parse_error(error_code code, size_t line, size_t col);
	private:
		std::string form_message(error_code code);
	};

	// результат выполенния парсинга
	struct parse_result {
		bool valid = false; // флаг удачного или нет результата
		json_value json_val; // значение json полученное в резульатате парсинга 
		std::vector<std::unique_ptr<base_error>> errors; // ошибки полученные при парсинге
	};

	namespace impl {

		using namespace io;
		using namespace io_base;
		using namespace encodings;

		// реализация dom-парсера
		class dom_parser_impl {
		public:

			// конструктор
			dom_parser_impl(i_input_ptr_ref input, parse_config & conf);
			
			// запуск парсинга
			parse_result parse();

		private:

			using err_vect = std::vector<std::unique_ptr<base_error>>;

			// состояния
			enum class state {
				_start = 0 , // начало
				_obj_begin, // начало объекта
				_key, // парсинг ключа
				_colon, // : 
				_obj_value, // значение в объекте
				_obj_next, // запятая когда парсим объект
				_arr_begin, // начало массива
				_arr_value, // значение в массиве
				_arr_next, // запятая когда парсим массив 
				_val, // значение
				_end,  // конец
				_err, // ошибка
				_obj_end, // конец объекта 
				_arr_end, // конец массива
			};

			// таблица переходов
			static constexpr state table[11][12] {

//                        {			        }		        число			     bool		        null			   строка			     [               ]              ,               :              конец
/* _start */	 { state::_obj_begin, state::_err,     state::_val,	      state::_val,       state::_val,		state::_val,	   state::_arr_begin, state::_err,     state::_err,	     state::_err,   state::_end, state::_err },
/* _obj_begin */ { state::_err,       state::_obj_end, state::_err,	      state::_err,       state::_err,		state::_key,	   state::_err,       state::_err,     state::_err,	     state::_err,   state::_err, state::_err },
/* _key */       { state::_err,       state::_err,     state::_err,	      state::_err,       state::_err,		state::_err,	   state::_err,       state::_err,     state::_err,	     state::_colon, state::_err, state::_err },
/* _colon */	 { state::_obj_begin, state::_err,     state::_obj_value, state::_obj_value, state::_obj_value, state::_obj_value, state::_arr_begin, state::_err,     state::_err,	     state::_err,   state::_err, state::_err },
/* _obj_value */ { state::_err,       state::_obj_end, state::_err,       state::_err,       state::_err,       state::_err,       state::_err,       state::_err,     state::_obj_next, state::_err,   state::_err, state::_err },
/* _obj_next */  { state::_err,       state::_err,     state::_err,       state::_err,       state::_err,       state::_key,       state::_err,       state::_err,     state::_err,      state::_err,   state::_err, state::_err },
/* _arr_begin */ { state::_arr_value, state::_arr_end, state::_arr_value, state::_arr_value, state::_arr_value, state::_arr_value, state::_arr_begin, state::_arr_end, state::_err,      state::_err,   state::_err, state::_err },
/* _arr_value */ { state::_err,       state::_err,     state::_err,       state::_err,       state::_err,       state::_err,       state::_err,       state::_arr_end, state::_arr_next, state::_err,   state::_err, state::_err },
/* _arr_next */  { state::_arr_value, state::_err,     state::_arr_value, state::_arr_value, state::_arr_value, state::_arr_value, state::_arr_value, state::_arr_end, state::_err,      state::_err,   state::_err, state::_err },
/* _val */       { state::_err,       state::_err,     state::_err,       state::_err,		 state::_err,       state::_err,       state::_err,       state::_err,     state::_err,      state::_err,   state::_end, state::_err },
/* _end */		 { state::_end,       state::_end,     state::_end,       state::_end,       state::_end,       state::_end,       state::_end,       state::_end,     state::_end,      state::_end,   state::_end, state::_end }
			};

			// преобразовние типа в индекс
			size_t token_type_to_index(token_type type);
			
			// добавляет ошибку в вектор или выбрасывает ее дальше
			void error_handler(token & cur, err_vect & errors, parse_error::error_code code);

			// обраотчик состояния ключ объекта
			void key_state_handler(token & cur);

			// обраотчик состояния значение(объекта, массива и обычного)
			void value_state_handler(token & cur);

			// обработчик выхода из объекта или массива
			void end_state_handler(token & cur);

			// запуск конечного автомата
			void run(err_vect & errors);

			state _state = state::_start; // начальное состояние автомата
			state _perv_state; // предыдущее состояние
			std::stack<std::reference_wrapper<json_value>> _ref_stack; // стек ссылок
			bool _collect_mode = false; // режим сбора ошибок
			i_input_processor_ptr _input_proc; // входные данные
			i_decoder_ptr _decoder; // декодер 
			tokenizer_ptr _tokenizer; // токенайзер
		};

	} // impl

} // json

#endif // __DRONJSON_PARSER_IMPL_
