#ifndef _DRONJSON_JSON_VAL_
#define _DRONJSON_JSON_VAL_

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <type_traits>
#include <memory>

#include "error.h"

namespace json {

	class json_value;

	namespace core {
		namespace impl {
			class json_storage;
		};
	};
	
	using jv_pointer = json_value*;
	using jv_reference = json_value&;
	using json_object = std::map<std::string, json_value>;
	using json_array = std::vector<json_value>;
	using json_pointer_array = std::vector<jv_pointer>;

	// типы значений 
	enum class value_type {
		_null,
		_bool,
		_int,
		_uint,
		_double,
		_string,
		_array,
		_object
	};

	class value_error : error {
	public:
		enum class error_code {

			// ошибки для json_storage
			_type_is_not_numberic = 0,
			
			// ошибки для класса json_value
			_type_is_not_array,
			_type_is_not_object,
			_type_is_not_object_or_array,
			_index_out_of_array_size,
			_element_not_found, 
			
			// ошибки для итераторов
			_is_not_object_iterator,
			_is_not_const_object_iterator,
			_is_not_array_iterator,
			_is_not_const_array_iterator,
			_is_empty_iterator,
			_is_not_mutable_iterator

		};

		value_error(error_code code);
	private:
		std::string form_message(error_code code);
	};

	// класс итератора
	class json_value_iterator {

		using array_iterator = json_array::iterator;
		using const_array_iterator = json_array::const_iterator;
		using object_iterator = json_object::iterator;
		using const_object_iterator = json_object::const_iterator;

		enum class _iterator_type;
	public:
		// конструкторы класса
		json_value_iterator();
		explicit json_value_iterator(const array_iterator & arrit);
		explicit json_value_iterator(const object_iterator & objit);
		explicit json_value_iterator(const const_array_iterator & arrit);
		explicit json_value_iterator(const const_object_iterator & objit);
		json_value_iterator(const json_value_iterator & other);

		~json_value_iterator();

		// движения вперед
		json_value_iterator & operator++();
		json_value_iterator operator++(int);
		
		// движение назад
		json_value_iterator & operator--();
		json_value_iterator operator--(int);

		// операторы доступа
		jv_reference operator*();
		const jv_reference operator*() const;
		jv_pointer operator->();
		const jv_pointer operator->() const;

		// операторы сравнения
		bool operator==(const json_value_iterator & other) const;
		bool operator!=(const json_value_iterator & other) const;

		// возвращает ключ
		std::string key() const;

		// возвращает значение
		jv_reference value();
		const jv_reference value() const;

		// проверка не является ли итератором массива
		bool is_array_iterator() const;

		// проверка не является ли константным итератором массива
		bool is_const_array_iterator() const;

		// проверка не является ли итератором объекта
		bool is_object_iterator() const;

		// проверка не является ли константным итератором объекта
		bool is_const_object_iterator() const;

		// возвращает итератор на массив
		array_iterator get_array_iterator() const;

		// возвращает константный итератор на массив
		const_array_iterator get_const_array_iterator() const;

		// возвращает итератор на объект
		object_iterator get_object_iterator() const;

		// возвращает константный итератор на объект
		const_object_iterator get_const_object_iterator() const;

		// возвращает тип
		_iterator_type type() const;

		// проверяет валидность итератора
		bool valid() const;

		// проврка на константность 
		bool is_const() const;

		// проверка на изменяемость
		bool is_mutable() const;

	private:

		enum class _iterator_type { _empty, _array, _object, c_array, c_object }; // тип итератора
		union data{
			array_iterator arr_it;
			const_array_iterator const_arr_it;
			object_iterator object_iter;
			const_object_iterator const_object_iter;

			data() : arr_it{} {};
			~data() {};
		};
		data _data;
		_iterator_type _type;
	};

	// класс занчения 
	class json_value {
	public:

		// конструтор класса
		json_value();
		json_value(value_type type);
		json_value(bool data);
		json_value(int data);
		json_value(unsigned int data);
		json_value(double data);
		json_value(const std::string & string);
		json_value(const char * string);
		json_value(json_array data);
		json_value(json_object data);
		json_value(const json_value & val);
		json_value(json_value && val);
		json_value(const char * name, const json_value & val);
		json_value(const char * name, json_value && val);

		// операторы присвоения
		json_value & operator=(const json_value & jval);
		json_value & operator=(json_value && jval);
		json_value & operator=(bool & val);
		json_value & operator=(char c);
		json_value & operator=(unsigned int num);
		json_value & operator=(int num);
		json_value & operator=(double num);
		json_value & operator=(const std::string & string);
		json_value & operator=(const char * string);
		json_value & operator=(json_array array);
		json_value & operator=(json_object object);

		// деструктор класса
		~json_value(); // to-do нужен ли 

		// операторы сравнения 
		bool operator==(const json_value & jval) const;
		bool operator!=(const json_value & jval) const;

		// для получения булевого значения 
		bool & as_bool();
		bool & as_bool() const;

		// для получения числового значения
		int & as_int();
		int & as_int() const;
		unsigned int & as_uint();
		unsigned int & as_uint() const;
		double & as_double();
		double & as_double() const;

		// для получения строки
		std::string & as_string();
		std::string & as_string() const;

		// для получения массива
		json_array * as_array();
		json_array * as_array() const;

		// для получения объекта
		json_object * as_object();
		json_object * as_object() const;

		// функции присвоения
		void assign(const json_value & jval);
		void assign(json_value && jval);
		void assign(bool val);
		void assign(char c);
		void assign(unsigned int num);
		void assign(int num);
		void assign(double num);
		void assign(const std::string & string);
		void assign(const char * string);
		void assign(json_array array);
		void assign(json_object object);

		// оператор доступа
		json_value & operator[](int index);
		const json_value & operator[](int index) const;
		json_value & operator[](const char * key);
		const json_value & operator[](const char * key) const;
		json_value & operator[](const std::string & key);
		const json_value & operator[](const std::string & key) const;

		// оператор доступа с проверкой
		json_value & at(int index);
		const json_value & at(int index) const;
		json_value & at(const char * key);
		const json_value & at(const char * key) const;
		json_value & at(const std::string & string);
		const json_value & at(const std::string & string) const;

		// возвращает занчение по имени
		jv_pointer find(const std::string & key);
		jv_pointer find(const char * key);
		const jv_pointer find(const std::string & key) const;
		const jv_pointer find(const char * key) const;

		// возвращает все значения по указанному ключу
		json_pointer_array select(const std::string & key);
		json_pointer_array select(const char * key);

		// проверяет наличие значения 
		bool contains(const std::string & key, bool deep_mode = false) const;
		bool contains(const char * key, bool deep_mode = false) const;

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

		// очистка (полное удаление данных)
		void clear();

		// удаляет с возвратом удаляемого значения
		json_value extract(const char * key);
		json_value extract(const std::string & key);
		json_value extract(int index);

		// возвращает тип
		value_type type() const;
		void type(value_type _t);

		// проверки на соответствие типу
		bool is_null() const;
		bool is_bool() const;
		bool is_int() const;
		bool is_uint() const;
		bool is_double() const;
		bool is_number() const;
		bool is_string() const;
		bool is_array() const;
		bool is_object() const;

		// возвращает общее число элементов
		size_t item_count();

		// возвращает колличество элементов в json_value
		size_t size() const noexcept;

		// возвращает true если пустой
		bool empty() const noexcept; 

		// возвращает итератор на начало объекта/массива 
		json_value_iterator begin();
		json_value_iterator begin() const;
		json_value_iterator cbegin() const;

		// возвращает итератор на конец объекта/массива 
		json_value_iterator end();
		json_value_iterator end() const;
		json_value_iterator cend() const;

	private:

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

		std::unique_ptr<json::core::impl::json_storage> _storage; // хранилище данных
	};

	namespace core {

		namespace impl {

			// вспомогательные структуры дя  преобразования типов value_type
			template<typename T> struct type_to_enum;
			template<> struct type_to_enum<bool> {
				static const value_type type = value_type::_bool;
			};
			template<> struct type_to_enum<int> {
				static const value_type type = value_type::_int;
			};
			template<> struct type_to_enum<unsigned int> {
				static const value_type type = value_type::_uint;
			};
			template<> struct type_to_enum<double> {
				static const value_type type = value_type::_double;
			};
			template<> struct type_to_enum<std::string> {
				static const value_type type = value_type::_string;
			};
			template<> struct type_to_enum<json_array> {
				static const value_type type = value_type::_array;
			};
			template<> struct type_to_enum<json_object> {
				static const value_type type = value_type::_object;
			};

			// класс для управления паматью json_value
			class json_storage {
			public:
				// конструктор класса
				json_storage() noexcept;
				json_storage(const json_storage & other);
				json_storage(json_storage && other) noexcept;

				// оператор присвоения 
				json_storage & operator=(const json_storage & other);
				json_storage & operator=(json_storage && other);

				// операторы сравнения 
				bool operator==(const json_storage & other) const;
				bool operator!=(const json_storage & other) const;

				// деструктор
				~json_storage();

				// очистка памяти
				void clear() noexcept;

				// возврат значения с необходимым типом
				template<typename data_type> data_type * get() noexcept {
					return get_ptr<data_type>();
				}

				// возврат константного значения с необходимым типом
				template<typename data_type> const data_type * get() const noexcept {
					return const_cast<json_storage*>(this)->get_ptr<data_type>();
				}

				// установка значения
				template<typename data_type, typename... args>
				void set(args&&... _args) {
					clear(); // очистка памяти перед инициализацией нового типа
					construct<data_type>(std::forward<args>(_args)...);
					_type = type_to_enum<data_type>::type;
				}

				// установка и возврат типа
				value_type type() const;
				void type(value_type new_type);

			private:

				union storage_union {
					bool bool_data;
					int int_data;
					unsigned uint_data;
					double double_data;
					std::string str_data;
					json_object object_data;
					json_array array_data;

					storage_union() noexcept : bool_data(false) {}
					~storage_union() {}
				};
				storage_union _storage;

				// доступ к полям 
				template<typename data_type> inline data_type * get_ptr() noexcept;

				// конструирование поля
				template <typename data_type, typename... args> inline void construct(args&&... _args) {
					construct_lvalue<data_type>(std::forward<args>(_args)...);
				}

				template <typename data_type, typename arg> inline void construct(const arg & _arg) {
					construct_rvalue<data_type>((data_type)_arg);
				}

				template <typename data_type, typename arg> inline void construct(arg & _arg) {
					construct_ref<data_type>((data_type)_arg);
				}

				template <typename data_type, typename arg> inline void construct(const arg * _arg) {
					construct_pointer<data_type>(_arg);
				}

				template <typename data_type, typename... args> inline void construct_lvalue(args&&... _args) { }
				template <typename data_type, typename arg> inline void construct_rvalue(const arg & _arg) { }
				template <typename data_type, typename arg> inline void construct_ref(arg & _arg) { }
				template <typename data_type, typename arg> inline void construct_pointer(const arg * _arg) { }

				// удаление данных поля
				template <typename data_type> void destroy();
				
				// копирование 
				void copy_data(const json_storage & other);

				// перемещение 
				void move_data(json_storage && other) noexcept;

				value_type _type; // тип
			};

			// сравнение числовых типов
			static bool compare_num_types(const json_storage & a, const json_storage & b);

			// проверка числовых типов
			static bool check_num_types(const value_type & a, const value_type & b);

			// преобразует в число
			static double cast_to_double(const json_storage & data);

			template<> inline bool * json_storage::get_ptr<bool>() noexcept { return &_storage.bool_data; }
			template<> inline int * json_storage::get_ptr<int>() noexcept { return &_storage.int_data; }
			template<> inline unsigned int * json_storage::get_ptr<unsigned int>() noexcept { return &_storage.uint_data; }
			template<> inline double * json_storage::get_ptr<double>() noexcept { return &_storage.double_data; }
			template<> inline std::string * json_storage::get_ptr<std::string>() noexcept { return &_storage.str_data; }
			template<> inline json_array * json_storage::get_ptr<json_array>() noexcept { return &_storage.array_data; }
			template<> inline json_object * json_storage::get_ptr<json_object>() noexcept { return &_storage.object_data; }

			template<> inline void json_storage::construct_lvalue<bool>(bool&& val) { _storage.bool_data = val; };
			template<> inline void json_storage::construct_lvalue<int>(int&& val) { _storage.int_data = val; };
			template<> inline void json_storage::construct_lvalue<unsigned int>(unsigned int&& val) { _storage.uint_data = val; };
			template<> inline void json_storage::construct_lvalue<double>(double&& val) { _storage.double_data = val; };
			template<> inline void json_storage::construct_lvalue<std::string>(std::string&& val) {
				new (&_storage.str_data) std::string(std::move(val));
			};
			template<> inline void json_storage::construct_lvalue<json_array>(json_array&& val) {
				new (&_storage.array_data) json_array(std::move(val));
			};
			template<> inline void json_storage::construct_lvalue<json_object>(json_object&& val) {
				new (&_storage.object_data) json_object(std::move(val));
			};

			template<> inline void json_storage::construct_rvalue<bool>(const bool& val) { _storage.bool_data = val; };
			template<> inline void json_storage::construct_rvalue<int>(const int& val) { _storage.int_data = val; };
			template<> inline void json_storage::construct_rvalue<unsigned int>(const unsigned int& val) { _storage.uint_data = val; };
			template<> inline void json_storage::construct_rvalue<double>(const double& val) { _storage.double_data = val; };
			template<> inline void json_storage::construct_rvalue<std::string>(const std::string& val) {
				new (&_storage.str_data) std::string(val);
			};
			template<> inline void  json_storage::construct_rvalue<json_array>(const json_array& val) {
				new (&_storage.array_data) json_array(val);
			};
			template<> inline void  json_storage::construct_rvalue<json_object>(const json_object& val) {
				new (&_storage.object_data) json_object(val);
			};

			template<> inline void  json_storage::construct_pointer<std::string>(const char* val) {
				new (&_storage.str_data) std::string(val);
			};

		    template<> inline void json_storage::construct_ref<bool>(bool& val) { _storage.bool_data = val; };
			template<> inline void json_storage::construct_ref<int>(int& val) { _storage.int_data = val; };
			template<> inline void json_storage::construct_ref<unsigned int>(unsigned int& val) { _storage.uint_data = val; };
			template<> inline void json_storage::construct_ref<double>(double& val) { _storage.double_data = val; };
			template<> inline void json_storage::construct_ref<std::string>(std::string& val) {
				new (&_storage.str_data) std::string(val);
			};
			template<> inline void json_storage::construct_ref<json_array>(json_array& val) {
				new (&_storage.array_data) json_array(val);
			};
			template<> inline void json_storage::construct_ref<json_object>(json_object& val) {
				new (&_storage.object_data) json_object(val);
			};

			template<> inline void json_storage::destroy<std::string>() {
				_storage.str_data.~basic_string();
			}
			template<> inline void json_storage::destroy<json_array>() {
				_storage.array_data.~vector();
			}
			template<> inline void json_storage::destroy<json_object>() {
				_storage.object_data.~map();
			}

		}
	}
}

#endif // !_DRONJSON_JSON_VALUE_