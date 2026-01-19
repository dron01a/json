#include "json_value.h"

json::json_value::json_value() : _type(value_type::_null) {}

json::json_value::json_value(bool data) {
	bool_data = data;
	_type = value_type::_bool;
}

json::json_value::json_value(double data) {
	num_data = data;
	_type = value_type::_number;
}

json::json_value::json_value(const std::string & string) {
	str_data = new std::string(string);
	_type = value_type::_string;
}

json::json_value::json_value(const char * string) {
	str_data = new std::string(string);
	_type = value_type::_string;
}

json::json_value::json_value(json_array data) {
	array_data = new json_array(data);
	_type = value_type::_array;
}

json::json_value::json_value(json_object data) {
	object_data = new json_object(data);
	_type = value_type::_object;
}

json::json_value::json_value(const json_value & val) {
	copy_data(val);
}

json::json_value::json_value(json_value && val) {
	move_data(std::move(val));
}

json::json_value::json_value(const char * name, const json_value & val) {
	object_data = new json_object();
	object_data->insert({ name, val });
	_type = value_type::_object;
}

json::json_value::json_value(const char * name, const json_value && val) {
	object_data = new json_object();
	object_data->insert({ name, std::move(val) });
	_type = value_type::_object;
}

json::json_value & json::json_value::operator=(const json_value & val) {
	copy_data(val);
	return *this;
}

json::json_value & json::json_value::operator=(json_value && val) {
	move_data(std::move(val));
	return *this;
}

json::json_value::~json_value() {
	clear_data();
}

bool & json::json_value::as_bool() {
	return bool_data;
}

bool & json::json_value::as_bool() const {
	return const_cast<json_value*>(this)->as_bool();
}

void json::json_value::as_bool(bool & val) {
	clear_data();
	_type = value_type::_bool;
	bool_data = val;
}

double & json::json_value::as_num() {
	return num_data;
}

double & json::json_value::as_num() const {
	return const_cast<json_value*>(this)->as_num();
}

void json::json_value::as_num(double num) {
	clear_data();
	_type = value_type::_number;
	num_data = num;
}

std::string & json::json_value::as_string() {
	return *str_data;
}

std::string & json::json_value::as_string() const {
	return const_cast<json_value*>(this)->as_string();
}

void json::json_value::as_string(const std::string & string) {
	clear_data();
	_type = value_type::_string;
	str_data = new std::string(string);
}

void json::json_value::as_string(const char * string) {
	clear_data();
	_type = value_type::_string;
	str_data = new std::string(string);
}

json::json_array * json::json_value::as_array() {
	return array_data;
}

json::json_array * json::json_value::as_array() const {
	return const_cast<json_value*>(this)->as_array();
}

void json::json_value::as_array(json_array array) {
	clear_data();
	_type = value_type::_array;
	array_data = new json_array(array);
}

json::json_object * json::json_value::as_object() {
	return object_data;
}

json::json_object * json::json_value::as_object() const {
	return const_cast<json_value*>(this)->as_object();
}

void json::json_value::as_object(json_object object) {
	clear_data();
	_type = value_type::_object;
	object_data = new json_object(object);
}

json::json_value * json::json_value::find(const std::string & name) {
	return find(name.c_str());
}

json::json_value * json::json_value::find(const char * name) {
	json_value * result = nullptr;
	switch (_type) {
	case json::value_type::_array:
		result = find_in_array(name);
		break;
	case json::value_type::_object:
		result = find_in_object(name);
		break;
	}
	return result;
}

json::json_value * json::json_value::add(const json_value & val) {
	json_value * result = nullptr;
	switch (_type) {
	case json::value_type::_array:
		array_data->push_back(val);
		result = &array_data->back();
		break;
	}
	return result;
}

json::json_value * json::json_value::add(json_value && val) {
	json_value * result = nullptr;
	switch (_type) {
	case json::value_type::_array:
		array_data->push_back(std::move(val));
		result = &array_data->back();
		break;
	}
	return result;
}

json::json_value * json::json_value::add(const char * name, const json_value & val) {
	json_value * result = nullptr;
	switch (_type) {
	case json::value_type::_object:
		object_data->insert_or_assign(name, val);
		result = &object_data->operator[](name);
		break;
	case json::value_type::_array:
		array_data->push_back(json_value(name, val));
		result = &array_data->back();
		break;
	}
	return result;
}

json::json_value * json::json_value::add(const char * name, json_value && val) {
	json_value * result = nullptr;
	switch (_type) {
	case json::value_type::_object:
		object_data->insert_or_assign(name, std::move(val));
		result = &object_data->operator[](name);
		break;
	case json::value_type::_array:
		array_data->push_back(json_value(name, std::move(val)));
		result = &array_data->back();
		break;
	}
	return result;
}

json::json_value * json::json_value::add(const std::string & name, const json_value & val) {
	return add(name.c_str(), val);
}

json::json_value * json::json_value::add(const std::string & name, json_value && val) {
	return add(name.c_str(), std::move(val));
}

json::value_type json::json_value::type() const {
	return _type;
}

void json::json_value::type(value_type _t) {
	_type = _t;
}

void json::json_value::clear_data() {
	switch (_type) {
	case json::value_type::_string:
		delete str_data;
		str_data = nullptr;
		break;
	case json::value_type::_array:
		delete array_data;
		array_data = nullptr;
		break;
	case json::value_type::_object:
		delete object_data;
		object_data = nullptr;
		break;
	}
}

void json::json_value::copy_data(const json_value & val) {
	clear_data();
	_type = val._type;
	switch (_type) {
	case value_type::_bool:
		bool_data = val.bool_data;
		break;
	case value_type::_number:
		num_data = val.num_data;
		break;
	case value_type::_string:
		str_data = new std::string(*val.str_data);
		break;
	case value_type::_array:
		array_data = new json_array(*val.array_data);
		break;
	case value_type::_object:
		object_data = new json_object(*val.object_data);
		break;
	}
}

void json::json_value::move_data(json_value && val) {
	clear_data();
	_type = std::move(val._type);
	switch (_type) {
	case value_type::_bool:
		bool_data = std::move(val.bool_data);
		break;
	case value_type::_number:
		num_data = std::move(val.num_data);
		break;
	case value_type::_string:
		std::swap(str_data, val.str_data);
		val.str_data = nullptr;
		break;
	case value_type::_array:
		std::swap(array_data, val.array_data);
		val.array_data = nullptr;
		break;
	case value_type::_object:
		std::swap(object_data, val.object_data);
		val.array_data = nullptr;
		break;
	}
}

json::json_value * json::json_value::find_in_array(const char * name) {
	json::json_value * _result = nullptr;
	for (size_t _i = 0; _i < array_data->size(); ++_i) {
		switch (array_data->operator[](_i)._type) {
		case json::value_type::_array:
			_result = array_data->operator[](_i).find_in_array(name);
			break;
		case json::value_type::_object:
			_result = array_data->operator[](_i).find_in_object(name);
			break;
		}
		if (_result) {
			break;
		}
	}
	return _result;
}

json::json_value * json::json_value::find_in_object(const char * name) {
	json::json_value * _result = nullptr;
	if (object_data->count(std::string(name)) != 0) {
		_result = &object_data->operator[](name);
	}
	if (!_result) {
		for (auto it = object_data->begin(); it != object_data->end(); ++it) {
			switch (it->second._type) {
			case json::value_type::_array:
				_result = it->second.find_in_array(name);
				break;
			case json::value_type::_object:
				_result = it->second.find_in_object(name);
				break;
			}
			if (_result) {
				break;
			}
		}
	}
	return _result;
}