#include "json_value.h"

using namespace json;

json_value_iterator::json_value_iterator() : _type(_iterator_type::_empty) {}

json_value_iterator::json_value_iterator(array_iterator & arrit) : _type(_iterator_type::_array) {
	_data.arr_it = arrit;
}

json_value_iterator::json_value_iterator(object_iterator & objit) : _type(_iterator_type::_object) {
	_data.object_iter = objit;
}

json_value_iterator::json_value_iterator(const_array_iterator & arrit) : _type(_iterator_type::c_array) {
	_data.const_arr_it = arrit;
}

json_value_iterator::json_value_iterator(const_object_iterator & objit) : _type(_iterator_type::c_object) {
	_data.const_object_iter = objit;
}

json_value_iterator::json_value_iterator(const json_value_iterator & other) : _type(other._type) {
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

json_value_iterator::~json_value_iterator() {}

json_value_iterator & json_value_iterator::operator++() {
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

json_value_iterator & json_value_iterator::operator++(int) {
	json_value_iterator temp = *this;
	++(*this);
	return temp;
}

json_value_iterator & json_value_iterator::operator--() {
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

json_value_iterator & json_value_iterator::operator--(int) {
	json_value_iterator temp = *this;
	--(*this);
	return temp;
}

jv_reference json_value_iterator::operator*() {
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

const jv_reference json_value_iterator::operator*() const {
	return const_cast<jv_reference>(this->operator*());
}

jv_pointer json_value_iterator::operator->() {
	return &(operator*());
}

const jv_pointer json_value_iterator::operator->() const {
	return &(operator*());
}

bool json_value_iterator::operator==(const json_value_iterator & other) const {
	if (_type != other._type) {
		return false;
	}
	switch (_type) {
	case json_value_iterator::_iterator_type::_empty:
		return true;
	case json_value_iterator::_iterator_type::_array:
		return this->_data.arr_it == other._data.arr_it;
	case json_value_iterator::_iterator_type::_object:
		return this->_data.object_iter == other._data.object_iter;
	case json_value_iterator::_iterator_type::c_array:
		return this->_data.const_arr_it == other._data.const_arr_it;
	case json_value_iterator::_iterator_type::c_object:
		return this->_data.const_object_iter == other._data.const_object_iter;
	}
}

bool json_value_iterator::operator!=(const json_value_iterator & other) const {
	return !(*this == other);
}

std::string json_value_iterator::key() const {
	switch (_type) {
	case json_value_iterator::_iterator_type::_object:
		return _data.object_iter->first;
	case json_value_iterator::_iterator_type::c_object:
		return _data.const_object_iter->first;
	default:
		return ""; // вставить выброс ошибок
	}
}

jv_reference json_value_iterator::value() {
	switch (_type) {
	case json_value_iterator::_iterator_type::_array:
		return *_data.arr_it;
	case json_value_iterator::_iterator_type::_object:
		return _data.object_iter->second;
	default:
		break; // вставить выброс ошибок
	}
}

const jv_reference json_value_iterator::value() const {
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

bool json_value_iterator::is_array_iterator() const {
	return _type == _iterator_type::_array;
}

bool json_value_iterator::is_const_array_iterator() const {
	return _type == _iterator_type::c_array;
}

bool json_value_iterator::is_object_iterator() const {
	return _type == _iterator_type::_object;
}

bool json_value_iterator::is_const_object_iterator() const {
	return _type == _iterator_type::c_object;
}

json_value_iterator::array_iterator json_value_iterator::get_array_iterator() const {
	if (_type == _iterator_type::_array) {
		return _data.arr_it;
	}
	throw; // ошибка
}

json_value_iterator::const_array_iterator json_value_iterator::get_const_array_iterator() const {
	if (_type == _iterator_type::c_array) {
		return _data.const_arr_it;
	}
	throw; // ошибка
}

json_value_iterator::object_iterator json_value_iterator::get_object_iterator() const {
	if (_type == _iterator_type::_object) {
		return _data.object_iter;
	}
	throw; // ошибка
}

json_value_iterator::const_object_iterator json_value_iterator::get_const_object_iterator() const {
	if (_type == _iterator_type::c_object) {
		return _data.const_object_iter;
	}
	throw; // ошибка
}

json_value_iterator::_iterator_type json_value_iterator::type() const {
	return _type;
}

bool json_value_iterator::valid() const {
	return _type != _iterator_type::_empty;
}

bool json_value_iterator::is_const() const {
	return _type == _iterator_type::c_array || _type == _iterator_type::c_object;
}

bool json_value_iterator::is_mutable() const {
	return _type == _iterator_type::_array || _type == _iterator_type::_object;
}

void json_value::clear_storage() noexcept {
	switch (_type) {
	case value_type::_string:
		get_ptr<std::string>()->~basic_string();
		break;
	case value_type::_array:
		get_ptr<json_array>()->~vector();
		break;
	case value_type::_object:
		get_ptr<json_object>()->~map();
		break;
	}
	_type = value_type::_null;
}

json_value::json_value(value_type type){
	_type = type;
	switch (_type){
	case value_type::_bool:
		construct<bool>(false);
		break;
	case value_type::_number:
		construct<double>(0);
		break;
	case value_type::_string:
		construct<std::string>("");
		break;
	case value_type::_array:
		construct<json_array>();
		break;
	case value_type::_object:
		construct<json_object>();
		break;
	}
}

json_value::json_value(bool data) {
	construct<bool>(data);
	_type = value_type::_bool;
}

json_value::json_value(double data) {
	construct<double>(data);
	_type = value_type::_number;
}

json_value::json_value(const std::string & string) {
	construct<std::string>(string);
	_type = value_type::_string;
}

json_value::json_value(const char * string) {
	construct<std::string>(string);
	_type = value_type::_string;
}

json_value::json_value(json_array data) {
	construct<json_array>(data);
	_type = value_type::_array;
}

json_value::json_value(json_object data) {
	construct<json_object>(data);
	_type = value_type::_object;
}

json_value::json_value(const json_value & val) {
	copy_data(val);
}

json_value::json_value(json_value && val) {
	move_data(std::move(val));
}

json_value::json_value(const char * name, const json_value & val) {
	construct<json_object>();
	get_ptr<json_object>()->insert({ name, val });
	_type = value_type::_object;
}

json_value::json_value(const char * name, const json_value && val) {
	construct<json_object>();
	get_ptr<json_object>()->insert({ name, val });
	_type = value_type::_object;
}

json_value & json_value::operator=(const json_value & val) {
	clear_storage();
	copy_data(val);
	return *this;
}

json_value & json_value::operator=(json_value && val) {
	clear_storage();
	move_data(std::move(val));
	return *this;
}

json_value::~json_value() {
	clear_storage();
}

bool & json_value::as_bool() {
	return *get_ptr<bool>();
}

bool & json_value::as_bool() const {
	return const_cast<json_value*>(this)->as_bool();
}

void json_value::as_bool(bool & val) {
	clear_storage();
	_type = value_type::_bool;
	construct<bool>(val);
}

double & json_value::as_num() {
	return *get_ptr<double>();
}

double & json_value::as_num() const {
	return const_cast<json_value*>(this)->as_num();
}

void json_value::as_num(double num) {
	clear_storage();
	_type = value_type::_number;
	construct<double>(num);
}

std::string & json_value::as_string() {
	return  *get_ptr<std::string>();
}

std::string & json_value::as_string() const {
	return const_cast<json_value*>(this)->as_string();
}

void json_value::as_string(const std::string & string) {
	clear_storage();
	_type = value_type::_string;
	construct<std::string>(string);
}

void json_value::as_string(const char * string) {
	clear_storage();
	_type = value_type::_string;
	construct<std::string>(string);
}

json_array * json_value::as_array() {
	return get_ptr<json_array>();
}

json_array * json_value::as_array() const {
	return const_cast<json_value*>(this)->as_array();
}

void json_value::as_array(json_array array) {
	clear_storage();
	_type = value_type::_array;
	construct<json_array>(array);
}

json_object * json_value::as_object() {
	return get_ptr<json_object>();
}

json_object * json_value::as_object() const {
	return const_cast<json_value*>(this)->as_object();
}

void json_value::as_object(json_object object) {
	clear_storage();
	_type = value_type::_object;
	construct<json_object>(object);
}

jv_pointer json_value::find(const std::string & name) {
	return find(name.c_str());
}

jv_pointer json_value::find(const char * name) {
	jv_pointer result = nullptr;
	switch (_type) {
	case value_type::_array:
		result = find_in_array(name);
		break;
	case value_type::_object:
		result = find_in_object(name);
		break;
	}
	return result;
}

json_pointer_array json_value::select(const std::string & key){
	return select(key.c_str());
}

json_pointer_array json_value::select(const char * key){
	json_pointer_array result;
	switch (_type) {
	case value_type::_array:
		result = select_in_array(key);
		break;
	case value_type::_object:
		result = select_in_object(key);
		break;
	}
	return result;
}

jv_pointer json_value::add(const json_value & val) {
	if (_type != value_type::_array) {
		return nullptr;
	}
	jv_pointer result = nullptr;
	switch (_type) {
	case value_type::_array:
		get_ptr<json_array>()->push_back(val);
		result = &get_ptr<json_array>()->back();
		break;
	}
	return result;
}

jv_pointer json_value::add(json_value && val) {
	if (_type != value_type::_array) {
		return nullptr;
	}
	jv_pointer result = nullptr;
	switch (_type) {
	case value_type::_array:
		get_ptr<json_array>()->push_back(std::move(val));
		result = &get_ptr<json_array>()->back();
		break;
	}
	return result;
}

jv_pointer json_value::add(const char * name, const json_value & val) {
	if (_type != value_type::_object) {
		return nullptr;
	}
	jv_pointer result = nullptr;
	switch (_type) {
	case value_type::_object:
		get_ptr<json_object>()->insert_or_assign(name, val);
		result = &get_ptr<json_object>()->operator[](name);
		break;
	case value_type::_array:
		get_ptr<json_array>()->push_back(json_value(name, val));
		result = &get_ptr<json_array>()->back();
		break;
	}
	return result;
}

jv_pointer json_value::add(const char * name, json_value && val) {
	if (_type != value_type::_object) {
		return nullptr;
	}
	jv_pointer result = nullptr;
	switch (_type) {
	case value_type::_object:
		get_ptr<json_object>()->insert_or_assign(name, std::move(val));
		result = &get_ptr<json_object>()->operator[](name);
		break;
	case value_type::_array:
		get_ptr<json_array>()->push_back(json_value(name, std::move(val)));
		result = &get_ptr<json_array>()->back();
		break;
	}
	return result;
}

jv_pointer json_value::add(const std::string & name, const json_value & val) {
	return add(name.c_str(), val);
}

jv_pointer json_value::add(const std::string & name, json_value && val) {
	return add(name.c_str(), std::move(val));
}

void json_value::remove(const char * key){
	if (_type != value_type::_object) {
		return;
	}
	auto target = get_ptr<json_object>()->find(key);
	if (target != get_ptr<json_object>()->end()) {
		get_ptr<json_object>()->erase(target);
	}
}

void json_value::remove(const std::string & key){
	remove(key.c_str());
}

void json_value::remove(size_t num){
	if (_type != value_type::_array || num < 0) {
		return;
	}
	if (num > get_ptr<json_array>()->size()) {
		return;
	}
	get_ptr<json_array>()->erase(get_ptr<json_array>()->begin() + num);
}

void json_value::clear(){
	clear_storage();
}

json_value json_value::extract(const char * key){
	if (_type != value_type::_object) {
		throw; // ошибка
		//return;
	}
	auto target = get_ptr<json_object>()->find(key);
	json_value res = std::move(target->second);
	if (target != get_ptr<json_object>()->end()) {
		get_ptr<json_object>()->erase(target);
	}
	return res;
}

json_value json_value::extract(const std::string & key){
	return extract(key.c_str());
}

value_type json_value::type() const {
	return _type;
}

void json_value::type(value_type _t) {
	clear_storage();
	_type = _t;
	switch (_type) {
	case value_type::_bool:
		construct<bool>(false);
		break;
	case value_type::_number:
		construct<double>(0);
		break;
	case value_type::_string:
		construct<std::string>("");
		break;
	case value_type::_array:
		construct<json_array>();
		break;
	case value_type::_object:
		construct<json_object>();
		break;
	}
}

bool json_value::is_null() const {
	return _type == value_type::_null;
}

bool json_value::is_bool() const {
	return _type == value_type::_bool;
}

bool json_value::is_number() const {
	return _type == value_type::_number;
}

bool json_value::is_string() const {
	return _type == value_type::_string;
}

bool json_value::is_array() const {
	return _type == value_type::_array;
}

bool json_value::is_object() const {
	return _type == value_type::_object;
}

size_t json_value::item_count(){
	size_t count = 0;
	switch (_type){
	case value_type::_null:
	case value_type::_number:
	case value_type::_bool:
	case value_type::_string:
		return 1;
	case value_type::_array:
		for (size_t i = 0; i < get_ptr<json_array>()->size(); ++i) {
			count += 1 + get_ptr<json_array>()->operator[](i).item_count();
		}
		break;
	case value_type::_object:
		for (auto it = get_ptr<json_object>()->begin(); it != get_ptr<json_object>()->end(); ++it) {
			count += 1 + it->second.item_count();
		}
		break;
	}
	return count;
}

json_value_iterator json_value::begin(){
	switch (_type){
	case value_type::_array:
		return json_value_iterator(get_ptr<json_array>()->begin());
	case value_type::_object:
		return json_value_iterator(get_ptr<json_object>()->begin());
	default:
		break; // вставить обработчик ошибки
	}
}

json_value_iterator json_value::begin() const {
	return cbegin();
}

json_value_iterator json_value::cbegin() const{
	switch (_type) {
	case value_type::_array:
		return json_value_iterator(get_ptr<json_array>()->cbegin());
	case value_type::_object:
		return json_value_iterator(get_ptr<json_object>()->cbegin());
	default:
		break; // вставить обработчик ошибки
	}
}

json_value_iterator json_value::end(){
	switch (_type) {
	case value_type::_array:
		return json_value_iterator(get_ptr<json_array>()->end());
	case value_type::_object:
		return json_value_iterator(get_ptr<json_object>()->end());
	default:
		break; // вставить обработчик ошибки
	}
}

json_value_iterator json_value::end() const{
	return cend();
}

json_value_iterator json_value::cend() const {
	switch (_type) {
	case value_type::_array:
		return json_value_iterator(get_ptr<json_array>()->cend());
	case value_type::_object:
		return json_value_iterator(get_ptr<json_object>()->cend());
	default:
		break; // вставить обработчик ошибки
	}
}

void json_value::copy_data(const json_value & val) {
	_type = val._type;
	switch (_type) {
	case value_type::_bool:
		construct<bool>(*val.get_ptr<bool>());
		break;
	case value_type::_number:
		construct<double>(*val.get_ptr<double>());
		break;
	case value_type::_string:
		construct<std::string>(*val.get_ptr<std::string>());
		break;
	case value_type::_array:
		construct<json_array>(*val.get_ptr<json_array>());
		break;
	case value_type::_object:
		construct<json_object>(*val.get_ptr<json_object>());
		break;
	}
}

void json_value::move_data(json_value && val) {
	_type = val._type;
	switch (_type) {
	case value_type::_bool:
		construct<bool>(std::move(*val.get_ptr<bool>()));
		break;
	case value_type::_number:
		construct<double>(std::move(*val.get_ptr<double>()));
		break;
	case value_type::_string:
		construct<std::string>(std::move(*val.get_ptr<std::string>()));
		val.get_ptr<std::string>()->~basic_string();
		break;
	case value_type::_array:
		construct<json_array>(std::move(*val.get_ptr<json_array>()));
		val.get_ptr<json_array>()->~vector();
		break;
	case value_type::_object:
		construct<json_object>(std::move(*val.get_ptr<json_object>()));
		val.get_ptr<json_object>()->~map();
		break;
	}
	val._type = value_type::_null;
}

jv_pointer json_value::find_impl(json_value & val, const char * name){
	switch (val._type) {
	case value_type::_array:
		return val.find_in_array(name);
	case value_type::_object:
		return val.find_in_object(name);
	}
	return nullptr;
}

jv_pointer json_value::find_in_array(const char * name) {
	jv_pointer _result = nullptr;
	for (size_t _i = 0; _i < get_ptr<json_array>()->size(); ++_i) {
		_result = find_impl(get_ptr<json_array>()->operator[](_i), name);
		if (_result) {
			break;
		}
	}
	return _result;
}

jv_pointer json_value::find_in_object(const char * name) {
	jv_pointer _result = nullptr;
	if (get_ptr<json_object>()->count(std::string(name)) != 0) {
		_result = &get_ptr<json_object>()->operator[](name);
	}
	if (!_result) {
		for (auto it = get_ptr<json_object>()->begin(); it != get_ptr<json_object>()->end(); ++it) {
			_result = find_impl(it->second, name);
			if (_result) {
				break;
			}
		}
	}
	return _result;
}

void json_value::select_impl(json_pointer_array & res, json_pointer_array & sub, json_value & val, const char * name){
	switch (val._type) {
	case value_type::_array:
		sub = val.select_in_array(name);
		break;
	case value_type::_object:
		sub = val.select_in_object(name);
		break;
	}
	if (sub.size() != 0) {
		res.insert(res.end(), sub.begin(), sub.end());
	}
}

json_pointer_array json_value::select_in_array(const char * name){
	json_pointer_array _result;
	json_pointer_array _sub_result;
	for (size_t _i = 0; _i < get_ptr<json_array>()->size(); ++_i) {
		select_impl(_result, _sub_result, get_ptr<json_array>()->operator[](_i), name);
	}
	return _result;
}

json_pointer_array json_value::select_in_object(const char * name){
	json_pointer_array _result;
	json_pointer_array _sub_result;
	if (get_ptr<json_object>()->count(std::string(name)) != 0) {
		_result.push_back(&get_ptr<json_object>()->operator[](name));
	}
	for (auto it = get_ptr<json_object>()->begin(); it != get_ptr<json_object>()->end(); ++it) {
		select_impl(_result, _sub_result, it->second, name);
	}
	return _result;
}
