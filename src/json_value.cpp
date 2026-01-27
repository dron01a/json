#include "json_value.h"

json::json_value_iterator::json_value_iterator() : _type(_iterator_type::_empty) {}

json::json_value_iterator::json_value_iterator(array_iterator & arrit) : _type(_iterator_type::_array) {
	_data.arr_it = arrit;
}

json::json_value_iterator::json_value_iterator(object_iterator & objit) : _type(_iterator_type::_object) {
	_data.object_iter = objit;
}

json::json_value_iterator::json_value_iterator(const_array_iterator & arrit) : _type(_iterator_type::c_array) {
	_data.const_arr_it = arrit;
}

json::json_value_iterator::json_value_iterator(const_object_iterator & objit) : _type(_iterator_type::c_object) {
	_data.const_object_iter = objit;
}

json::json_value_iterator::json_value_iterator(const json_value_iterator & other) : _type(other._type) {
	switch (other._type) {
	case _iterator_type::_array:
		_data.arr_it = other._data.arr_it;
		break;
	case _iterator_type::c_array:
		_data.const_arr_it = other._data.const_arr_it;
		break;
	case _iterator_type::_object:
		_data.object_iter = other._data.object_iter;
		break;
	case _iterator_type::c_object:
		_data.const_object_iter = other._data.const_object_iter;
		break;
	}
}

json::json_value_iterator::~json_value_iterator() {}

json::json_value_iterator & json::json_value_iterator::operator++() {
	switch (_type) {
	case _iterator_type::_array:
		_data.arr_it++;
		break;
	case _iterator_type::c_array:
		_data.const_arr_it++;
		break;
	case _iterator_type::_object:
		_data.object_iter++;
		break;
	case _iterator_type::c_object:
		_data.const_object_iter++;
		break;
	}
	return *this;
}

json::json_value_iterator & json::json_value_iterator::operator++(int) {
	json_value_iterator temp = *this;
	++(*this);
	return temp;
}

json::json_value_iterator & json::json_value_iterator::operator--() {
	switch (_type) {
	case _iterator_type::_array:
		_data.arr_it--;
		break;
	case _iterator_type::c_array:
		_data.const_arr_it--;
		break;
	case _iterator_type::_object:
		_data.object_iter--;
		break;
	case _iterator_type::c_object:
		_data.const_object_iter--;
		break;
	}
	return *this;
}

json::json_value_iterator & json::json_value_iterator::operator--(int) {
	json_value_iterator temp = *this;
	--(*this);
	return temp;
}

json::jv_reference json::json_value_iterator::operator*() {
	switch (_type) {
	case _iterator_type::_array:
		return *_data.arr_it;
	case _iterator_type::c_array:
		return const_cast<jv_reference>(*_data.const_arr_it);
	case _iterator_type::_object:
		return _data.object_iter->second;
	case _iterator_type::c_object:
		return const_cast<jv_reference>(_data.const_object_iter->second);
	}
}

const json::jv_reference json::json_value_iterator::operator*() const {
	return const_cast<jv_reference>(this->operator*());
}

json::jv_pointer json::json_value_iterator::operator->() {
	return &(operator*());
}

const json::jv_pointer json::json_value_iterator::operator->() const {
	return &(operator*());
}

bool json::json_value_iterator::operator==(const json_value_iterator & other) const {
	if (_type != other._type) {
		return false;
	}
	switch (_type) {
	case json::json_value_iterator::_iterator_type::_empty:
		return true;
	case json::json_value_iterator::_iterator_type::_array:
		return this->_data.arr_it == other._data.arr_it;
	case json::json_value_iterator::_iterator_type::_object:
		return this->_data.object_iter == other._data.object_iter;
	case json::json_value_iterator::_iterator_type::c_array:
		return this->_data.const_arr_it == other._data.const_arr_it;
	case json::json_value_iterator::_iterator_type::c_object:
		return this->_data.const_object_iter == other._data.const_object_iter;
	}
}

bool json::json_value_iterator::operator!=(const json_value_iterator & other) const {
	return !(*this == other);
}

std::string json::json_value_iterator::key() const {
	switch (_type) {
	case json::json_value_iterator::_iterator_type::_object:
		return _data.object_iter->first;
	case json::json_value_iterator::_iterator_type::c_object:
		return _data.const_object_iter->first;
	default:
		return ""; // вставить выброс ошибок
	}
}

json::jv_reference json::json_value_iterator::value() {
	switch (_type) {
	case json::json_value_iterator::_iterator_type::_array:
		return *_data.arr_it;
	case json::json_value_iterator::_iterator_type::_object:
		return _data.object_iter->second;
	default:
		break; // вставить выброс ошибок
	}
}

const json::jv_reference json::json_value_iterator::value() const {
	switch (_type) {
	case _iterator_type::_array:
		return *_data.arr_it;
	case _iterator_type::c_array:
		return const_cast<jv_reference>(*_data.const_arr_it);
	case _iterator_type::_object:
		return _data.object_iter->second;
	case _iterator_type::c_object:
		return const_cast<jv_reference>(_data.const_object_iter->second);
	}
	throw; // вставить выброс ошибок
}

bool json::json_value_iterator::is_array_iterator() const {
	return _type == _iterator_type::_array;
}

bool json::json_value_iterator::is_const_array_iterator() const {
	return _type == _iterator_type::c_array;
}

bool json::json_value_iterator::is_object_iterator() const {
	return _type == _iterator_type::_object;
}

bool json::json_value_iterator::is_const_object_iterator() const {
	return _type == _iterator_type::c_object;
}

json::json_value_iterator::array_iterator json::json_value_iterator::get_array_iterator() const {
	if (_type == _iterator_type::_array) {
		return _data.arr_it;
	}
	throw; // ошибка
}

json::json_value_iterator::const_array_iterator json::json_value_iterator::get_const_array_iterator() const {
	if (_type == _iterator_type::c_array) {
		return _data.const_arr_it;
	}
	throw; // ошибка
}

json::json_value_iterator::object_iterator json::json_value_iterator::get_object_iterator() const {
	if (_type == _iterator_type::_object) {
		return _data.object_iter;
	}
	throw; // ошибка
}

json::json_value_iterator::const_object_iterator json::json_value_iterator::get_const_object_iterator() const {
	if (_type == _iterator_type::c_object) {
		return _data.const_object_iter;
	}
	throw; // ошибка
}

json::json_value_iterator::_iterator_type json::json_value_iterator::type() const {
	return _type;
}

bool json::json_value_iterator::valid() const {
	return _type != _iterator_type::_empty;
}

bool json::json_value_iterator::is_const() const {
	return _type == _iterator_type::c_array || _type == _iterator_type::c_object;
}

bool json::json_value_iterator::is_mutable() const {
	return _type == _iterator_type::_array || _type == _iterator_type::_object;
}

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

json::jv_pointer json::json_value::find(const std::string & name) {
	return find(name.c_str());
}

json::jv_pointer json::json_value::find(const char * name) {
	jv_pointer result = nullptr;
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

json::json_pointer_array json::json_value::select(const std::string & key){
	return select(key.c_str());
}

json::json_pointer_array json::json_value::select(const char * key){
	json_pointer_array result;
	switch (_type) {
	case json::value_type::_array:
		result = select_in_array(key);
		break;
	case json::value_type::_object:
		result = select_in_object(key);
		break;
	}
	return result;
}

json::jv_pointer json::json_value::add(const json_value & val) {
	if (_type != value_type::_array) {
		return nullptr;
	}
	jv_pointer result = nullptr;
	switch (_type) {
	case json::value_type::_array:
		array_data->push_back(val);
		result = &array_data->back();
		break;
	}
	return result;
}

json::jv_pointer json::json_value::add(json_value && val) {
	if (_type != value_type::_array) {
		return nullptr;
	}
	jv_pointer result = nullptr;
	switch (_type) {
	case json::value_type::_array:
		array_data->push_back(std::move(val));
		result = &array_data->back();
		break;
	}
	return result;
}

json::jv_pointer json::json_value::add(const char * name, const json_value & val) {
	if (_type != value_type::_object) {
		return nullptr;
	}
	jv_pointer result = nullptr;
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

json::jv_pointer json::json_value::add(const char * name, json_value && val) {
	if (_type != value_type::_object) {
		return nullptr;
	}
	jv_pointer result = nullptr;
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

json::jv_pointer json::json_value::add(const std::string & name, const json_value & val) {
	return add(name.c_str(), val);
}

json::jv_pointer json::json_value::add(const std::string & name, json_value && val) {
	return add(name.c_str(), std::move(val));
}

void json::json_value::remove(const char * key){
	if (_type != value_type::_object) {
		return;
	}
	auto target = object_data->find(key);
	if (target != object_data->end()) {
		object_data->erase(target);
	}
}

void json::json_value::remove(const std::string & key){
	remove(key.c_str());
}

void json::json_value::remove(size_t num){
	if (_type != value_type::_array || num < 0) {
		return;
	}
	if (num > array_data->size()) {
		return;
	}
	array_data->erase(array_data->begin() + num);
}

void json::json_value::clear(){
	clear_data();
}

json::json_value json::json_value::extract(const char * key){
	if (_type != value_type::_object) {
		throw; // ошибка
		//return;
	}
	auto target = object_data->find(key);
	json_value res = std::move(target->second);
	if (target != object_data->end()) {
		object_data->erase(target);
	}
	return res;
}

json::json_value json::json_value::extract(const std::string & key){
	return extract(key.c_str());
}

json::value_type json::json_value::type() const {
	return _type;
}

void json::json_value::type(value_type _t) {
	_type = _t;
}

bool json::json_value::is_null() const {
	return _type == value_type::_null;
}

bool json::json_value::is_bool() const {
	return _type == value_type::_bool;
}

bool json::json_value::is_number() const {
	return _type == value_type::_number;
}

bool json::json_value::is_string() const {
	return _type == value_type::_string;
}

bool json::json_value::is_array() const {
	return _type == value_type::_array;
}

bool json::json_value::is_object() const {
	return _type == value_type::_object;
}

size_t json::json_value::item_count(){
	size_t count = 0;
	switch (_type){
	case value_type::_null:
	case value_type::_number:
	case value_type::_bool:
	case value_type::_string:
		return 1;
	case value_type::_array:
		for (size_t i = 0; i < array_data->size(); ++i) {
			count += 1 + array_data->operator[](i).item_count();
		}
		break;
	case value_type::_object:
		for (auto it = object_data->begin(); it != object_data->end(); ++it) {
			count += 1 + it->second.item_count();
		}
		break;
	}
	return count;
}

json::json_value_iterator json::json_value::begin(){
	switch (_type){
	case json::value_type::_array:
		return json_value_iterator(array_data->begin());
	case json::value_type::_object:
		return json_value_iterator(object_data->begin());
	default:
		break; // вставить обработчик ошибки
	}
}

json::json_value_iterator json::json_value::begin() const {
	return cbegin();
}

json::json_value_iterator json::json_value::cbegin() const{
	switch (_type) {
	case json::value_type::_array:
		return json_value_iterator(array_data->cbegin());
	case json::value_type::_object:
		return json_value_iterator(object_data->cbegin());
	default:
		break; // вставить обработчик ошибки
	}
}

json::json_value_iterator json::json_value::end(){
	switch (_type) {
	case json::value_type::_array:
		return json_value_iterator(array_data->end());
	case json::value_type::_object:
		return json_value_iterator(object_data->end());
	default:
		break; // вставить обработчик ошибки
	}
}

json::json_value_iterator json::json_value::end() const{
	return cend();
}

json::json_value_iterator json::json_value::cend() const {
	switch (_type) {
	case json::value_type::_array:
		return json_value_iterator(array_data->cend());
	case json::value_type::_object:
		return json_value_iterator(object_data->cend());
	default:
		break; // вставить обработчик ошибки
	}
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

json::jv_pointer json::json_value::find_impl(json_value & val, const char * name){
	switch (val._type) {
	case json::value_type::_array:
		return val.find_in_array(name);
	case json::value_type::_object:
		return val.find_in_object(name);
	}
	return nullptr;
}

json::jv_pointer json::json_value::find_in_array(const char * name) {
	json::jv_pointer _result = nullptr;
	for (size_t _i = 0; _i < array_data->size(); ++_i) {
		_result = find_impl(array_data->operator[](_i), name);
		if (_result) {
			break;
		}
	}
	return _result;
}

json::jv_pointer json::json_value::find_in_object(const char * name) {
	json::jv_pointer _result = nullptr;
	if (object_data->count(std::string(name)) != 0) {
		_result = &object_data->operator[](name);
	}
	if (!_result) {
		for (auto it = object_data->begin(); it != object_data->end(); ++it) {
			_result = find_impl(it->second, name);
			if (_result) {
				break;
			}
		}
	}
	return _result;
}

void json::json_value::select_impl(json_pointer_array & res, json_pointer_array & sub, json_value & val, const char * name){
	switch (val._type) {
	case json::value_type::_array:
		sub = val.select_in_array(name);
		break;
	case json::value_type::_object:
		sub = val.select_in_object(name);
		break;
	}
	if (sub.size() != 0) {
		res.insert(res.end(), sub.begin(), sub.end());
	}
}

json::json_pointer_array json::json_value::select_in_array(const char * name){
	json_pointer_array _result;
	json_pointer_array _sub_result;
	for (size_t _i = 0; _i < array_data->size(); ++_i) {
		select_impl(_result, _sub_result, array_data->operator[](_i), name);
	}
	return _result;
}

json::json_pointer_array json::json_value::select_in_object(const char * name){
	json_pointer_array _result;
	json_pointer_array _sub_result;
	if (object_data->count(std::string(name)) != 0) {
		_result.push_back(&object_data->operator[](name));
	}
	for (auto it = object_data->begin(); it != object_data->end(); ++it) {
		select_impl(_result, _sub_result, it->second, name);
	}
	return _result;
}