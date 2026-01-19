#ifndef __DRONJSON__JS_VAL__
#define __DRONJSON__JS_VAL__

#include <string>
#include <vector>
#include <map>

namespace json {

	class json_value;

	using json_object = std::map<std::string, json_value>;
	using json_array = std::vector<json_value>;

	// типы значений 
	enum class value_type {
		_null,
		_bool,
		_number,
		_string,
		_array,
		_object
	};

	// класс занчения 
	class json_value {
	public:
		// конструтор класса
		json_value();
		json_value(bool data);
		json_value(double data);
		json_value(const std::string & string);
		json_value(const char * string);
		json_value(json_array data);
		json_value(json_object data);
		json_value(const json_value & val);
		json_value(json_value && val);
		json_value(const char * name, const json_value & val);
		json_value(const char * name, const json_value && val);

		json_value & operator=(const json_value & jval);
		json_value & operator=(json_value && jval);

		// деструктор класса
		~json_value();

		// для получения булевого значения 
		bool & as_bool();
		bool & as_bool() const;
		void as_bool(bool & val);

		// для получения числового значения
		double & as_num();
		double & as_num() const;
		void as_num(double num);

		// для получения строки
		std::string & as_string();
		std::string & as_string() const;
		void as_string(const std::string & string);
		void as_string(const char * string);

		// для получения массива
		json_array * as_array();
		json_array * as_array() const;
		void as_array(json_array array);

		// для получения объекта
		json_object * as_object();
		json_object * as_object() const;
		void as_object(json_object object);

		// возвращает занчение по имени
		json_value * find(const std::string & name);
		json_value * find(const char * name);

		// добавляет элемент в json (при условии что масиив)
		json_value * add(const json_value & val);
		json_value * add(json_value && val);

		// добавляет элемент в json (при условии что объект)
		json_value * add(const char * name, const json_value & val);
		json_value * add(const char * name, json_value && val);
		json_value * add(const std::string & name, const json_value & val);
		json_value * add(const std::string & name, json_value && val);

		// возвращает тип
		value_type type() const;
		void type(value_type _t);

	private:

		// очистка данных в зависимости от типа
		void clear_data();

		// перемещение данных 
		void copy_data(const json_value & val);

		// копирование данных
		void move_data(json_value && val);

		// поиск заначения в массиве 
		json_value * find_in_array(const char * name);

		// поиск значения в объекте
		json_value * find_in_object(const char * name);

		value_type _type = value_type::_null; // тип

		// данные
		union {
			bool bool_data;
			double num_data;
			std::string  * str_data;
			json_object * object_data;
			json_array * array_data;
		};
	};

}

#endif