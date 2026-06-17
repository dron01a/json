#include <iostream>
#include <cassert>

#include "json_value.h"

#include "../tools/tests_framework.h"

using namespace json;
using namespace json::core::impl;

TEST_CASE(test_assign) {
	json_storage _storage;
	_storage.set<int>(153);
	TEST_ASSERT(_storage.type() == value_type::_int);
	TEST_ASSERT(*_storage.get<int>() == 153);
	*_storage.get<int>() = 23;
	TEST_ASSERT(*_storage.get<int>() == 23);
	_storage.set<std::string>("test data");
	TEST_ASSERT(_storage.type() == value_type::_string);
	TEST_ASSERT(*_storage.get<std::string>() == "test data");
	json_object _obj = { { "a", 2.2 }, { "b", "test" } };
	_storage.set<json_object>(_obj);
	TEST_ASSERT(_storage.type() == value_type::_object);
	TEST_ASSERT(_storage.get<json_object>()->operator[]("a") == json_value(2.2));
	TEST_ASSERT(_storage.get<json_object>()->operator[]("b") == json_value("test"));
}

TEST_CASE(test_compare_operators) {
	json_storage _storage1, _storage2, _storage3;
	_storage1.set<int>(12353);
	_storage2.set<int>(12353);
	_storage3.set<std::string>("test str");
	TEST_ASSERT(_storage1 == _storage2);
	_storage1.set<int>(53);
	_storage2.set<int>(12353);
	TEST_ASSERT(_storage1 != _storage2);
	TEST_ASSERT(_storage1 != _storage3);
	TEST_ASSERT(_storage2 != _storage3);
	json_array arr1 = { 2.2, 4, "str" };
	json_array arr2 = { 2.2, 4, "str" };
	_storage1.set<json_array>(arr1);
	_storage2.set<json_array>(arr2);
	TEST_ASSERT(_storage1 == _storage2);
	json_array arr3 = { "www", 4, "str" };
	_storage2.set<json_array>(arr3);
	TEST_ASSERT(_storage1 != _storage2);
	json_object obj1 = { { "a", 1289 },{ "b", "data" } };
	json_object obj2 = { { "a", 1289 },{ "b", "data" } };
	_storage1.set<json_object>(obj1);
	_storage2.set<json_object>(obj2);
	TEST_ASSERT(_storage1 == _storage2);
}

TEST_CASE(test_copy) {
	json_storage _storage1, _storage2, _storage3;
	_storage1.set<int>(12353);
	_storage3.set<std::string>("test str");
	_storage2 = _storage1;
	_storage1 = _storage3;
	TEST_ASSERT(*_storage2.get<int>() == 12353);
	TEST_ASSERT(_storage1 != _storage2);
	TEST_ASSERT(*_storage1.get<std::string>() == "test str");
	TEST_ASSERT(_storage1 == _storage3);
}

TEST_CASE(test_move) {
	json_storage _storage1, _storage2, _storage3;
	_storage1.set<int>(12353);
	_storage3.set<std::string>("test str");
	_storage2 = std::move(_storage1);
	_storage1 = std::move(_storage3);
	TEST_ASSERT(*_storage2.get<int>() == 12353);
	TEST_ASSERT(_storage1 != _storage2);
	TEST_ASSERT(*_storage1.get<std::string>() == "test str");
	TEST_ASSERT(_storage3.type() == value_type::_null);
}

TEST_CASE(clear_test) {
	json_storage _storage;
	_storage.set<std::string>("data");
	TEST_ASSERT(*_storage.get<std::string>() == "data");
	_storage.clear();
	TEST_ASSERT(_storage.type() == value_type::_null);
}

TEST_CASE(test_array) {
	json_storage _storage;
	json_array src_arr = { 2.2, 4, "str" };
	_storage.set<json_array>(src_arr);
	TEST_ASSERT(_storage.get<json_array>()->size() == 3);
	TEST_ASSERT(_storage.type() == value_type::_array);
	TEST_ASSERT(_storage.get<json_array>()->operator[](0) == json_value(2.2));
	TEST_ASSERT(_storage.get<json_array>()->operator[](1) == json_value(4));
	TEST_ASSERT(_storage.get<json_array>()->operator[](2) == json_value("str"));
	auto arr = _storage.get<json_array>();
	arr->operator[](0).assign(95);
	arr->operator[](1).assign("str 1");
	TEST_ASSERT(_storage.get<json_array>()->operator[](0) == json_value(95));
	TEST_ASSERT(_storage.get<json_array>()->operator[](1) == json_value("str 1"));
	TEST_ASSERT(_storage.get<json_array>()->size() == 3);
}

TEST_CASE(test_object) {
	json_storage _storage;
	json_object _obj = { { "a", 1289 },{ "b", "data" } };
	_storage.set<json_object>(_obj);
	TEST_ASSERT(_storage.type() == value_type::_object);
	TEST_ASSERT(_storage.get<json_object>()->operator[]("a") == json_value(1289));
	TEST_ASSERT(_storage.get<json_object>()->operator[]("b") == json_value("data"));
	auto obj = _storage.get<json_object>();
	obj->operator[]("b") = 2;
	obj->operator[]("c") = "new string data";
	TEST_ASSERT(_storage.get<json_object>()->size() == 3);
	TEST_ASSERT(_storage.get<json_object>()->operator[]("b") == json_value(2));
	TEST_ASSERT(_storage.get<json_object>()->operator[]("c") == json_value("new string data"));
}

TEST_CASE(test_number) {
	json_storage _storage1, _storage2, _storage3;
	_storage1.set<int>(2);
	_storage2.set<double>(2.0);
	_storage3.set<unsigned int>((unsigned int)2);
	TEST_ASSERT(_storage1 == _storage2);
	TEST_ASSERT(_storage2 == _storage3);
	TEST_ASSERT(_storage1 == _storage3);
}


#ifndef _ALL_TESTS_RUN

int main() {
	tester::inst().run();
	return 0;
}

#endif // _ALL_TESTS_RUN
