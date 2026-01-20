#ifndef __DRONJSON__
#define __DRONJSON__

#include "error.h"
#include "json_value.h"
#include "json_reader.h"
#include "json_writer.h"
#include "json_parser.h"

namespace json {

	// класс для работы с json-файлом как с документом
	class json_doc {
	public:

		// конструктор класса
		json_doc(); 
		json_doc(const json_value & val);
		json_doc(json_value && val);
		json_doc(const json_doc & doc);
		json_doc(json_doc && doc);

		~json_doc();

		// оператор присвоения
		json_doc & operator=(const json_doc & doc);
		json_doc & operator=(json_doc && doc);

		// загрузка из файла или строки
		void load(const char * data_string, bool is_file = false);

		// загрузка из потока
		void load(std::istream & stream);

		// сохранить как файл
		void save(const char * data_string);

		// сохранить в поток
		void save(std::ostream & stream);

		// сохранить как строку
		std::string to_string();

		// возвращает последнюю полученную ошибку
		error get_last_error();

		// возвращает корневой элемент 
		json_value * root();

		// осуществляет поиск элементов 
		bool has(const char * key);

		// возвращает элемент по ключу 
		json_value * get(const char * key);

		// возвращает все элементы по данному ключу
	//	std::vector<json_value*> select(const char * key);

	private:

		json_parser _parser; // парсер 
		json_writer _writer; // для записи
		std::vector<error> _errors; // вектор с ошибками
		json_value * _root; // корневой элемент 

	};

};

#endif // ! __DRONJSON__
