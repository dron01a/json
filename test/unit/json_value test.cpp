#include <iostream>
#include <cassert>
#include "json_value.h"

#include "../tools/tests_framework.h"

using namespace json;
using namespace json::impl;

TEST_CASE(value_type_constructor) {
	json_value val1(value_type::_int);
	TEST_ASSERT(val1.is_int());
	TEST_ASSERT(val1.type() == value_type::_int);
	TEST_ASSERT(val1.size() == 1);
	json_value val2(value_type::_string);
	TEST_ASSERT(val2.is_string());
	TEST_ASSERT(val2.type() == value_type::_string);
	TEST_ASSERT(val2.size() != 1);
}

TEST_CASE(null_constructor) {
	json_value val1;
	TEST_ASSERT(val1.is_null());
	TEST_ASSERT(val1.type() == value_type::_null);
	TEST_ASSERT(val1.size() == 0);
}

TEST_CASE(bool_constructor) {
	json_value val1(false);
	TEST_ASSERT(val1.is_bool());
	TEST_ASSERT(val1.type() == value_type::_bool);
	TEST_ASSERT(val1.as_bool() != true);
	TEST_ASSERT(val1.as_bool() == false);
	json_value val2(true);
	TEST_ASSERT(val2.is_bool());
	TEST_ASSERT(val2.type() == value_type::_bool);
	TEST_ASSERT(val2.as_bool() == true);
	TEST_ASSERT(val2.as_bool() != false);
}

TEST_CASE(int_constructor) {
	json_value val1(456546);
	TEST_ASSERT(val1.is_int());
	TEST_ASSERT(val1.type() == value_type::_int);
	TEST_ASSERT(val1.as_int() != 2563);
	TEST_ASSERT(val1.as_int() == 456546);
}

TEST_CASE(unsigned_int_constructor) {
	json_value val1((unsigned int)7896);
	TEST_ASSERT(val1.is_uint());
	TEST_ASSERT(val1.type() == value_type::_uint);
	TEST_ASSERT(val1.type() != value_type::_int);
	TEST_ASSERT(val1.as_uint() != 2563);
	TEST_ASSERT(val1.as_uint() == 7896);
}

TEST_CASE(double_constructor) {
	json_value val1(-562.465);
	TEST_ASSERT(val1.is_double());
	TEST_ASSERT(val1.type() == value_type::_double);
	TEST_ASSERT(val1.as_uint() != 2563);
	TEST_ASSERT(val1.as_double() == -562.465);
}

TEST_CASE(string_constructor) {
	std::string my_str = "any text";
	json_value val1(my_str);
	TEST_ASSERT(val1.is_string());
	TEST_ASSERT(val1.type() == value_type::_string);
	TEST_ASSERT(val1.as_string() != "abra codabra");
	TEST_ASSERT(val1.as_string() == my_str);
	my_str = "abra codabra";
	json_value val2(my_str.c_str());
	TEST_ASSERT(val2.is_string());
	TEST_ASSERT(val2.type() == value_type::_string);
	TEST_ASSERT(val2.as_string() == "abra codabra");
	TEST_ASSERT(val2.as_string() != "any text");
}

TEST_CASE(array_constructor) {
	json_array my_arr = { 2.2, "str1", "str2", false, 123 };
	json_value val1(my_arr);
	TEST_ASSERT(val1.is_array());
	TEST_ASSERT(val1.type() == value_type::_array);
	TEST_ASSERT(val1.size() == 5);
	TEST_ASSERT(val1.as_array()->at(0) == 2.2);
	TEST_ASSERT(val1.as_array()->at(1) == "str1");
	TEST_ASSERT(val1.as_array()->at(2) == "str2");
	TEST_ASSERT(val1.as_array()->at(3) == false);
	TEST_ASSERT(val1.as_array()->at(4) == 123);
}

TEST_CASE(object_constructor) {
	json_object my_obj = { { "a", 1 }, { "b", 2 }, { "c", { "sub", 23 } } };
	json_value val1(my_obj);
	TEST_ASSERT(val1.is_object());
	TEST_ASSERT(val1.type() == value_type::_object);
	TEST_ASSERT(val1.size() == 3);
	TEST_ASSERT(val1.as_object()->at("a") == 1);
	TEST_ASSERT(val1.as_object()->at("b") == 2);
	TEST_ASSERT(val1.as_object()->at("c") == json_value("sub", 23));
}

TEST_CASE(copy_constructor) {
	json_object my_obj = { { "a", 1 },{ "b", 2 },{ "c",{ "sub", 23 } } };
	json_value src(my_obj);
	json_value dest(src);
	TEST_ASSERT(dest.is_object());
	TEST_ASSERT(dest.type() == value_type::_object);
	TEST_ASSERT(dest.size() == 3);
	TEST_ASSERT(dest.as_object()->at("a") == 1);
	TEST_ASSERT(dest.as_object()->at("b") == 2);
	TEST_ASSERT(dest.as_object()->at("c") == json_value("sub", 23));
}

TEST_CASE(move_constructor) {
	json_object my_obj = { { "a", 1 },{ "b", 2 },{ "c",{ "sub", 23 } } };
	json_value src(my_obj);
	json_value dest(std::move(src));
	TEST_ASSERT(!src.is_object());
	TEST_ASSERT(src.type() != value_type::_object);
	TEST_ASSERT(src.type() == value_type::_null);
	TEST_ASSERT(src.size() != 3);
	TEST_ASSERT(dest.is_object());
	TEST_ASSERT(dest.type() == value_type::_object);
	TEST_ASSERT(dest.size() == 3);
	TEST_ASSERT(dest.as_object()->at("a") == 1);
	TEST_ASSERT(dest.as_object()->at("b") == 2);
	TEST_ASSERT(dest.as_object()->at("c") == json_value("sub", 23));
}

TEST_CASE(sub_object_constructor) {
	json_value src(2);
	json_value val1("data", src);
	TEST_ASSERT(val1.is_object());
	TEST_ASSERT(val1.type() == value_type::_object);
	TEST_ASSERT(val1.type() != value_type::_null);
	TEST_ASSERT(val1["data"] == 2);
	json_value val2("data", 2);
	TEST_ASSERT(val2.is_object());
	TEST_ASSERT(val2.type() == value_type::_object);
	TEST_ASSERT(val2.type() != value_type::_null);
	TEST_ASSERT(val2["data"] == 2);
	json_value val3("data", std::move(src));
	TEST_ASSERT(src.type() == value_type::_null);
	TEST_ASSERT(val3.is_object());
	TEST_ASSERT(val3.type() == value_type::_object);
	TEST_ASSERT(val3.type() != value_type::_null);
	TEST_ASSERT(val3["data"] == 2);
}

TEST_CASE(assign_operators) {
	json_value val1;
	TEST_ASSERT(val1.is_null());
	val1 = 2;
	TEST_ASSERT(val1.is_int());
	TEST_ASSERT(val1.type() == value_type::_int);
	val1 = { val1 , val1 , val1 };
	TEST_ASSERT(val1.is_array());
	TEST_ASSERT(val1.type() == value_type::_array);
	TEST_ASSERT(val1.size() == 3);
	val1 = "string";
	TEST_ASSERT(val1.is_string());
	TEST_ASSERT(val1.type() == value_type::_string);
	TEST_ASSERT(val1.size() == 6);
	json_value val2 = std::move(val1);
	TEST_ASSERT(val1.is_null());
	TEST_ASSERT(val2.is_string());
	TEST_ASSERT(val2.type() == value_type::_string);
	TEST_ASSERT(val2.size() == 6);
}

TEST_CASE(assign_functions) {
	json_value val1;
	TEST_ASSERT(val1.is_null());
	val1.assign(2);
	TEST_ASSERT(val1.is_int());
	TEST_ASSERT(val1.type() == value_type::_int);
	val1.assign({ val1 , val1 , val1 });
	TEST_ASSERT(val1.is_array());
	TEST_ASSERT(val1.type() == value_type::_array);
	TEST_ASSERT(val1.size() == 3);
	val1.assign("string");
	TEST_ASSERT(val1.is_string());
	TEST_ASSERT(val1.type() == value_type::_string);
	TEST_ASSERT(val1.size() == 6);
	json_value val2;
	val2.assign(std::move(val1));
	TEST_ASSERT(val1.is_null());
	TEST_ASSERT(val2.is_string());
	TEST_ASSERT(val2.type() == value_type::_string);
	TEST_ASSERT(val2.size() == 6);
}

TEST_CASE(compare_operators) {
	json_value val1;
	TEST_ASSERT(val1.is_null());
	val1 = "string";
	TEST_ASSERT(val1.is_string());
	TEST_ASSERT(val1.type() == value_type::_string);
	TEST_ASSERT(val1.size() == 6);
	json_value val2 = val1;
	TEST_ASSERT(val2.is_string());
	TEST_ASSERT(val2.type() == value_type::_string);
	TEST_ASSERT(val2.size() == 6);
	TEST_ASSERT(val2 == val1);
	val1 = "string2";
	TEST_ASSERT(val2 != val1);
	val1 = 2.0;
	val2 = 2;
	TEST_ASSERT(val1.is_double());
	TEST_ASSERT(val2.is_int());
	TEST_ASSERT(val2 == val1);
}

TEST_CASE(get_operator) {
	json_object my_obj = { { "a", 1 },{ "b", 2 },{ "c", { "sub", 23 } } };
	json_value val1(my_obj);
	TEST_ASSERT(val1["a"] == 1);
	TEST_ASSERT(val1["b"] == 2);
	TEST_ASSERT(val1["c"] == json_value("sub", 23));
	val1["d"] = "test val";
	val1["e"] = 5;
	TEST_ASSERT(val1["d"] == "test val");
	TEST_ASSERT(val1["e"] == 5);
	TEST_ASSERT(val1.size() == 5);
	json_array my_arr = { 1, 2, "test" };
	json_value val2(my_arr);
	TEST_ASSERT(val2[0] == 1);
	TEST_ASSERT(val2[1] == 2);
	TEST_ASSERT(val2[2] == "test");
	val2[4] = "a";
	TEST_ASSERT(val2[4] == "a");
	TEST_ASSERT(val2.size() == 5);
}

TEST_CASE(at_operator) {
	json_object my_obj = { { "a", 1 }, { "b", 2 }, { "c", { "sub", 23 } } };
	json_value val1(my_obj);
	TEST_ASSERT(val1.at("a") == 1);
	TEST_ASSERT(val1.at("b") == 2);
	TEST_ASSERT(val1.at("c") == json_value("sub", 23));
	TEST_ASSERT_THROW(val1.at("d"), value_error);
	TEST_ASSERT_THROW(val1.at("e"), value_error);
	TEST_ASSERT(val1.size() == 3);
	json_array my_arr = { 1, 2, "test" };
	json_value val2(my_arr);
	TEST_ASSERT(val2.at(0) == 1);
	TEST_ASSERT(val2.at(1) == 2);
	TEST_ASSERT(val2.at(2) == "test");
	TEST_ASSERT_THROW(val2.at(4), value_error);
	TEST_ASSERT_THROW(val2.at(-5464), value_error);
	TEST_ASSERT_THROW(val2.at(12), value_error);
	TEST_ASSERT(val2.size() != 5);
}

TEST_CASE(find) {
	json_object my_obj = { { "a", 1 }, { "b", 2 }, { "c", { "sub", 23 } } };
	json_value val1(my_obj);
	auto res = val1.find("a");
	TEST_ASSERT(res != nullptr);
	TEST_ASSERT(*res == 1);
	res = val1.find("b");
	TEST_ASSERT(res != nullptr);
	TEST_ASSERT(*res == 2);
	res = val1.find("d");
	TEST_ASSERT(res == nullptr);
	res = val1.find("sub");
	TEST_ASSERT(res != nullptr);
	TEST_ASSERT(*res == 23);
}

TEST_CASE(select) {
	json_object my_obj = { { "point_a", { "x", 1 } }, 
						   { "point_b", { "x", 2 } }, 
						   { "point_c", { "x", 3 } } };
	json_value val1(my_obj);
	auto res = val1.select("x");
	TEST_ASSERT(res.size() != 0);
	TEST_ASSERT(res.size() == 3);
	TEST_ASSERT(*res[0] == 1);
	TEST_ASSERT(*res[1] == 2);
	TEST_ASSERT(*res[2] == 3);
}

TEST_CASE(add_from_array) {
	json_array my_arr = { 1, 2, "test" };
	json_value val(my_arr);
	auto arr_p = val.as_array();
	val.add(2);
	TEST_ASSERT(arr_p->size() == 4);
	TEST_ASSERT(arr_p->at(3) == 2);
	json_value val_from_move = "string";
	val.add(std::move(val_from_move));
	TEST_ASSERT(arr_p->size() == 5);
	TEST_ASSERT(arr_p->at(4) == "string");
	TEST_ASSERT(val_from_move.is_null());
	val.add("test", 123);
	TEST_ASSERT(arr_p->size() == 6);
	TEST_ASSERT(arr_p->at(5).is_object());
	TEST_ASSERT_THROW(arr_p->at(5).at("asd"), value_error);
	TEST_ASSERT(arr_p->at(5).at("test") == 123);
	json_value str_val = "string value";
	json_value int_val = 465;
	json_value bool_val = false;
	TEST_ASSERT_THROW(str_val.add(2), value_error);
	TEST_ASSERT_THROW(int_val.add(2), value_error);
	TEST_ASSERT_THROW(bool_val.add(2), value_error);
}

TEST_CASE(add_from_object) {
	json_object my_obj = { { "a", 1 },{ "b", 2 },{ "c", { "sub", 23 } } };
	json_value val(my_obj);
	auto obj_p = val.as_object();
	val.add("d", 5);
	val.add("e", false);
	val.add("f", "test");
	TEST_ASSERT(obj_p->size() == 6);
	TEST_ASSERT(obj_p->at("d") == 5);
	TEST_ASSERT(obj_p->at("e") == false);
	TEST_ASSERT(obj_p->at("f") == "test");
	json_value val_from_move = "string";
	val.add("moved", std::move(val_from_move));
	TEST_ASSERT(obj_p->size() == 7);
	TEST_ASSERT(obj_p->at("moved") == "string");
	TEST_ASSERT(val_from_move.is_null());
	json_value str_val = "string value";
	json_value int_val = 465;
	json_value bool_val = false;
	TEST_ASSERT_THROW(str_val.add(2), value_error);
	TEST_ASSERT_THROW(int_val.add(2), value_error);
	TEST_ASSERT_THROW(bool_val.add(2), value_error);
}

TEST_CASE(remove_from_array) {
	json_array my_arr = { 1, 2, "test" };
	json_value val(my_arr);
	auto arr_p = val.as_array();
	val.remove(1);
	TEST_ASSERT(arr_p->size() == 2);
	TEST_ASSERT_THROW(val.at(2), value_error);
	TEST_ASSERT_THROW(val.remove("4"), value_error);
	TEST_ASSERT_THROW(val.remove(-1), value_error);
	json_value str_val = "string value";
	json_value int_val = 465;
	json_value bool_val = false;
	TEST_ASSERT_THROW(str_val.remove(1), value_error);
	TEST_ASSERT_THROW(int_val.remove(1), value_error);
	TEST_ASSERT_THROW(bool_val.remove(1), value_error);
}

TEST_CASE(remove_from_object) {
	json_object my_obj = { { "a", 1 },{ "b", 2 },{ "c",{ "sub", 23 } } };
	json_value val(my_obj);
	auto obj_p = val.as_object();
	val.remove("a");
	TEST_ASSERT(obj_p->size() == 2);
	TEST_ASSERT_THROW(val.at("a"), value_error);
	json_value str_val = "string value";
	json_value int_val = 465;
	json_value bool_val = false;
	TEST_ASSERT_THROW(str_val.remove("a"), value_error);
	TEST_ASSERT_THROW(int_val.remove("a"), value_error);
	TEST_ASSERT_THROW(bool_val.remove("a"), value_error);
}

TEST_CASE(clear_data) {
	json_object my_obj = { { "a", 1 },{ "b", 2 },{ "c",{ "sub", 23 } } };
	json_value val(my_obj);
	val.clear();
	TEST_ASSERT(val.is_null());
	TEST_ASSERT(val.size() == 0);
	TEST_ASSERT_THROW(val.remove("a"), value_error);
	TEST_ASSERT_THROW(val.remove(1), value_error);
}

TEST_CASE(extract_from_array) {
	json_array my_arr = { 1, 2, "test" };
	json_value val(my_arr);
	auto arr_p = val.as_array();
	json_value res = val.extract(2);
	TEST_ASSERT(res == "test");
	TEST_ASSERT(arr_p->size() == 2);
	TEST_ASSERT_THROW(val.extract(-1), value_error);
	TEST_ASSERT_THROW(val.extract("a"), value_error);
}

TEST_CASE(extract_from_object) {
	json_object my_obj = { { "a", 1 },{ "b", 2 },{ "c",{ "sub", 23 } } };
	json_value val(my_obj);
	auto obj_p = val.as_object();
	json_value res = val.extract("a");
	TEST_ASSERT(res == 1);
	TEST_ASSERT(obj_p->size() == 2);
	TEST_ASSERT_THROW(val.at("a"), value_error);
	TEST_ASSERT_THROW(val.extract(1), value_error);
}

TEST_CASE(type_switch) {
	json_value val;
	val.type(value_type::_bool);
	TEST_ASSERT(val.is_bool());
	TEST_ASSERT(val == false);
	val.type(value_type::_int);
	TEST_ASSERT(val.is_int());
	TEST_ASSERT(val == 0);
	val.type(value_type::_double);
	TEST_ASSERT(val.is_double());
	TEST_ASSERT(val == 0);
	val.type(value_type::_string);
	TEST_ASSERT(val.is_string());
	TEST_ASSERT(val == "");
}

TEST_CASE(item_count) {
	json_object my_obj = { { "a", 1 },{ "b", 2 },{ "c",{ "sub", 23 } } };
	json_value val(my_obj);
	TEST_ASSERT(val.item_count() == 4);
	val = "test";
	TEST_ASSERT(val.item_count() == 1);
	json_array my_arr = { 1, 2, "test", { "a", 3 } };
	val = my_arr;
	TEST_ASSERT(val.item_count() == 5);
}

int main() {
	tester::inst().run();
	return 0;
}