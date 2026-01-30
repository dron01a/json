#ifndef __DRONJSON__ERROR__
#define __DRONJSON__ERROR__

#include <string>

namespace json {

	enum class error_category {
		io_error,
		encoding_error,
		parse_error,
		document_error,
		validation_error,
		value_error
	};

	// базовая структура ошибки
	class base_error : public std::exception {
	public: 
		base_error(error_category category, size_t line, size_t col, const std::string & message);

		// возращает линию ошибки 
		size_t line() const;

		// возвращает столбец ошибки
		size_t column() const;

		// возвращает категорию 
		error_category category() const;

		// возврат сообщения об ошибке
		const char * what() const noexcept override;

	private:

		// форматирование сообщения 
		std::string format_message() const;

		size_t _col = 0; // столбец
		size_t _line = 0; // линия 
		std::string _message; // сообщение об ошибке 
		error_category _error_cat; // категоррия ошибки 
	};

}


#endif