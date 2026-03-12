#include <iostream>
#include <cassert>
#include "json_value.h"

using namespace json;
using namespace json::impl;

#define my_assert(expr,  message) assert((void(message), expr))

// тест присвоения элементов 
void test_assign();

// тест операторов сравнения 
void test_compare_operators();

// тест копирования
void test_copy();

// тест перемещения 
void test_move();

// тест очистки 
void clear_test();

// тест работы с массивами 
void test_array();

// тест работы с объектами
void test_object();

int main() {
	std::cout << "json::impl::json_storage test begin" << std::endl;
	test_assign();
	test_compare_operators();
	test_copy();
	test_move();
	clear_test();
	test_array();
	test_object();
	std::cout << "json::impl::json_storage test passed" << std::endl;
	return 0;
}

// тест присвоения элементов 
void test_assign() {
	std::cout << "\tassign test begin" << std::endl;

	json_storage _storage;
	_storage.set<int>(153);
	my_assert(_storage.type() == value_type::_number, "type detect error | type is not number");
	my_assert(*_storage.get<int>() == 153, "wrong value");
	*_storage.get<int>() = 23;
	assert(*_storage.get<int>() == 23, "wrong value");
	std::cout << "\t\tassign int passed" << std::endl;

	_storage.set<std::string>("test data");

	my_assert(_storage.type() == value_type::_string, "type detect error | type is not string");
	my_assert(*_storage.get<std::string>() == "test data", "wrong value");
	std::cout << "\t\tassign string passed" << std::endl;

	json_object _obj = { { "a", 2.2 },{ "b", "test" } };
	_storage.set<json_object>(_obj);
	my_assert(_storage.type() == value_type::_object, "type detect error | type is not object");
	my_assert(_storage.get<json_object>()->operator[]("a") == json_value(2.2), "wrong value");
	my_assert(_storage.get<json_object>()->operator[]("b") == json_value("test"), "wrong value");
	std::cout << "\t\tassign json_object passed" << std::endl;

	std::cout << "\tassign test passed" << std::endl;
}

// тест операторов сравнения 
void test_compare_operators() {
	std::cout << "\tcompare test begin" << std::endl;

	json_storage _storage1, _storage2, _storage3;
	_storage1.set<int>(12353);
	_storage2.set<int>(12353);
	_storage3.set<std::string>("test str");
	my_assert(_storage1 == _storage2, "compare error");
	std::cout << "\t\toperator == test passed" << std::endl;

	_storage1.set<int>(53);
	_storage2.set<int>(12353);
	my_assert(_storage1 != _storage2, "compare error");
	my_assert(_storage1 != _storage3, "compare error");
	my_assert(_storage2 != _storage3, "compare error");
	std::cout << "\t\toperator != test passed" << std::endl;

	std::cout << "\tcompare test passed" << std::endl;
}

// тест копирования
void test_copy() {
	std::cout << "\tcopy test begin" << std::endl;
	json_storage _storage1, _storage2, _storage3;
	_storage1.set<int>(12353);
	_storage3.set<std::string>("test str");
	_storage2 = _storage1;
	_storage1 = _storage3;
	my_assert(*_storage2.get<int>() == 12353, "copy error");
	my_assert(_storage1 != _storage2, "copy error");
	my_assert(*_storage1.get<std::string>() == "test str", "copy error");
	my_assert(_storage1 == _storage3, "copy error");
	std::cout << "\tcopy test passed" << std::endl;
}

// тест перемещения 
void test_move() {
	std::cout << "\tmove test begin" << std::endl;
	json_storage _storage1, _storage2, _storage3;
	_storage1.set<int>(12353);
	_storage3.set<std::string>("test str");
	_storage2 = std::move(_storage1);
	_storage1 = std::move(_storage3);
	my_assert(*_storage2.get<int>() == 12353, "move error");
	my_assert(_storage1 != _storage2, "move error");
	my_assert(*_storage1.get<std::string>() == "test str", "move error");
	my_assert(_storage3.type() == value_type::_null, "move error");
	std::cout << "\tmove test passed" << std::endl;
}

// тест очистки 
void clear_test() {
	json_storage _storage;
	std::cout << "\tclear test begin" << std::endl;
	_storage.set<std::string>("data");
	my_assert(*_storage.get<std::string>() == "data", "assign error");
	_storage.clear();
	my_assert(_storage.type() == value_type::_null, "clear error");
	std::cout << "\tclear test passed" << std::endl;
}

// тест работы с массивами 
void test_array() {
	std::cout << "\tarray test begin" << std::endl;
	json_storage _storage;
	json_array src_arr = { 2.2, 4, "str" };
	_storage.set<json_array>(src_arr);
	my_assert(_storage.get<json_array>()->size() == 3, "array assign error");
	my_assert(_storage.type() == value_type::_array, "array assign error");
	my_assert(_storage.get<json_array>()->operator[](0) == json_value(2.2), "array assign error");
	my_assert(_storage.get<json_array>()->operator[](1) == json_value(4), "array assign error");
	my_assert(_storage.get<json_array>()->operator[](2) == json_value("str"), "array assign error");
	std::cout << "\t\tarray assign test passed" << std::endl;
	auto arr = _storage.get<json_array>();
	arr->operator[](0).assign(95);
	arr->operator[](1).assign("str 1");
	my_assert(_storage.get<json_array>()->operator[](0) == json_value(95), "array modify error");
	my_assert(_storage.get<json_array>()->operator[](1) == json_value("str 1"), "array modify error");
	my_assert(_storage.get<json_array>()->size() == 3, "array modify error");
	std::cout << "\t\tarray modify test passed" << std::endl;
	std::cout << "\tarray test passed" << std::endl;
}

// тест работы с объектами
void test_object() {
	std::cout << "\tarray test begin" << std::endl;
	json_storage _storage;
	json_object _obj = { { "a", 1289 },{ "b", "data" } };
	_storage.set<json_object>(_obj);
	my_assert(_storage.type() == value_type::_object, "object assign error");
	my_assert(_storage.get<json_object>()->operator[]("a") == json_value(1289), "object assign error | wrong value");
	my_assert(_storage.get<json_object>()->operator[]("b") == json_value("data"), "object assign error | wrong value");
	std::cout << "\t\tobject assign test passed" << std::endl;
	auto obj = _storage.get<json_object>();
	obj->operator[]("b") = 2;
	obj->operator[]("c") = "new string data";
	my_assert(_storage.get<json_object>()->size() == 3, "object modify error");
	my_assert(_storage.get<json_object>()->operator[]("b") == json_value(2), "object assign error | wrong value");
	my_assert(_storage.get<json_object>()->operator[]("c") == json_value("new string data"), "object assign error | wrong value");
	std::cout << "\t\tobject modify test passed" << std::endl;
	std::cout << "\tarray test passed" << std::endl;
}