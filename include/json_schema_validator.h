#ifndef _DRONJSON_VALIDATOR_
#define _DRONJSON_VALIDATOR_

#include <regex>

#include "core\error.h"
#include "core\json_value.h"

namespace json {

	enum class validation_error_code {
		incorrect_type = 0,
		no_matches_enum, 
		no_matches_const, 
		value_is_less_as_minimum,
		value_is_greater_as_maximum,
		not_multiple_of,
		str_len_is_less_as_minimum,
		str_len_is_greater_as_maximum,
		str_no_pattern,
		not_match_format_data_time,
		not_match_format_data,
		not_match_format_time,
		array_size_less_as_minimum,
		array_size_greater_as_maximum,
		array_item_no_match, // ??? надо оно
		array_size_no_match,
		array_items_is_not_unique,
		array_havent_contains_item,
		object_properties_less_as_minimum,
		object_properties_greater_as_maximum,
		object_required,
		object_no_have_property,
		object_pattern_properties,
		object_additional_properties,
		object_dependencies,
	};

	// класс ошибки валидации 
	struct validation_error {
		validation_error(validation_error_code code, std::string err_path) : error_code(code), path(err_path) {}
		validation_error_code error_code; // код ошибке
		std::string path; // путь к ошибке 
	};

	// класс валидатора
	class json_schema_validator {
	public:

		// конструктор
		json_schema_validator(const json_value & scheme, size_t max_errors = 100);

		// валидация
		bool validate(const json_value & inst);

		// вовзращает ощибки полученные в результате последней валидации
		std::vector<validation_error> errors();

	private:

		// провидит валидацию значения
		void validate_value(const json_value & scheme, const json_value & value);

		// проверяет тип
		void validate_type(const json_value & scheme, const json_value & value);

		// проверяет enum
		void validate_enum(const json_value & scheme, const json_value & value);

		// проверяет константы
		void validate_const(const json_value & scheme, const json_value & value);

		// проверка чисел
		void validate_number(const json_value & scheme, const json_value & value);

		// проверка строк 
		void validate_string(const json_value & scheme, const json_value & value);

		// проверка format
		void validate_format(const json_value & scheme, const json_value & value);

		// проверка массивов
		void validate_array(const json_value & scheme, const json_value & value);

		// проверка объектов
		void validate_object(const json_value & scheme, const json_value & value);

		// обработка поля схемы items при валидации массива 
		void array_items_handler(const json_value & items_scheme, json_array & arr);

		// обработка поля схемы uniqueItems при валидации массива 
		void array_uniqueItems_handler(const json_value & field, json_array & arr);

		// обработка поля схемы additionalItems при валидации массива 
		void array_additionalItems_handler(jv_pointer items, const json_value & field, json_array & arr);

		// обработка поля схемы contains при валидации массива 
		void array_contains_handler(const json_value & field, json_array & arr);

		// обработка поля схемы propertyNames при валидации объекта 
		void object_propertyNames_handler(const json_value & field, json_object & obj);

		// обработка поля схемы properties при валидации объекта 
		void object_properties_handler(const json_value & field, json_object & obj);

		// обработка поля схемы patternProperties при валидации объекта 
		void object_patternProperties_handler(const json_value & field, json_object & obj);

		// обработка поля схемы additionalProperties при валидации объекта (поле properties)
		void object_properties_additional_handler(const json_value & field, const json_value & properties, json_object & obj);

		// обработка поля схемы additionalProperties при валидации объекта (поле patternProperties)
		void object_patternProperties_additional_handler(const json_value & field, const json_value & properties, json_object & obj);

		// обработка поля схемы required при валидации объекта 
		void object_required_handler(const json_value & field, json_object & obj);

		// обработка поля схемы dependencies при валидации объекта 
		void object_dependencies_handler(const json_value & field, json_object & obj);

		// преобразует тип в строку 
		std::string type_to_string(value_type type);

		// формирует строку с путем к элементу
		void push_path(std::string name);

		// перемещение вверх по текущему пути 
		void pop_path();

		// добавляет ошибку к списку 
		void add_error(validation_error_code err);

		// преобразует json_value в double
		double jv_to_d(const json_value & value);

		// проверяет строку на соответсвие регулярному выжению
		void check_regex(const std::string & str, const std::string & regex_str, validation_error_code code);

		size_t _max_errors_count; // максимальное колличество ошибок
		json_value _scheme; // схема
		std::vector<validation_error> _errors; // ошибки 
		std::string cur_path; // текущий путь

		struct valid_end {};
	};
}

#endif // ! _DRONJSON_VALIDATOR_