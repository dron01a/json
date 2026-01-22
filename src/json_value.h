#ifndef __DRONJSON__JS_VAL__
#define __DRONJSON__JS_VAL__

#include <string>
#include <vector>
#include <map>

namespace json {

	class json_value;

	using jv_pointer = json_value*;
	using json_object = std::map<std::string, json_value>;
	using json_array = std::vector<json_value>;
	using json_pointer_array = std::vector<jv_pointer>;

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

		using array_iterator = json_array::iterator;
		using object_iterator = json_object::iterator;

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
		jv_pointer find(const std::string & key);
		jv_pointer find(const char * key);

		// возвращает все значения по указанному ключу
		json_pointer_array select(const std::string & key);
		json_pointer_array select(const char * key);

		// добавляет элемент в json (при условии что масиив)
		jv_pointer add(const json_value & val);
		jv_pointer add(json_value && val);

		// добавляет элемент в json (при условии что объект)
		jv_pointer add(const char * key, const json_value & val);
		jv_pointer add(const char * key, json_value && val);
		jv_pointer add(const std::string & key, const json_value & val);
		jv_pointer add(const std::string & key, json_value && val);

		// удаляет элемент по ключу
		void remove(const char * key);
		void remove(const std::string & key);

		// удаляет элемент по индексу
		void remove(size_t num);

		// очистка 
		void clear();

		// удаляет с возвратом удаляемого значения
		json_value extract(const char * key);
		json_value extract(const std::string & key);

		// возвращает тип
		value_type type() const;
		void type(value_type _t);

		// проверки на соответствие типу
		bool is_null() const;
		bool is_bool() const;
		bool is_number() const;
		bool is_string() const;
		bool is_array() const;
		bool is_object() const;

		// возвращает общее число элементов
		size_t item_count();

	private:

		// очистка данных в зависимости от типа
		void clear_data();

		// перемещение данных 
		void copy_data(const json_value & val);

		// копирование данных
		void move_data(json_value && val);

		// реализация всех значений по указанному ключу
		jv_pointer find_impl(json_value & val, const char * name);

		// поиск заначения в массиве 
		jv_pointer find_in_array(const char * name);

		// поиск значения в объекте
		jv_pointer find_in_object(const char * name);

		// реализация всех значений по указанному ключу
		void select_impl(json_pointer_array & res, json_pointer_array & sub,json_value & val, const char * name);

		// поиск всех значений по указанному ключу в массиве
		json_pointer_array select_in_array(const char * name);

		// поиск всех значений по указанному ключу в объекте
		json_pointer_array select_in_object(const char * name);

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