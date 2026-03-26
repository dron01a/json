#include "core\json_value.h"

using namespace json;
using namespace json::core::impl;

value_error::value_error(error_code code) : error(error_category::value_error, 0, 0, form_message(code)) {}

std::string value_error::form_message(error_code code) {
	switch (code) {
	case json::value_error::error_code::_type_is_not_numberic:
		return "is not numberic type";
	case json::value_error::error_code::_type_is_not_array:
		return "is not array";
	case json::value_error::error_code::_type_is_not_object:
		return "is not object";
	case json::value_error::error_code::_type_is_not_object_or_array:
		return "is not object or array";
	case json::value_error::error_code::_index_out_of_array_size:
		return "index outside of borders of array";
	case json::value_error::error_code::_element_not_found:
		return "element not found";
	case json::value_error::error_code::_is_not_object_iterator:
		return "is not object iterator";
	case json::value_error::error_code::_is_not_const_object_iterator:
		return "is not const object iterator";
	case json::value_error::error_code::_is_not_array_iterator:
		return "is not array iterator";
	case json::value_error::error_code::_is_not_const_array_iterator:
		return "is not const array iterator";
	case json::value_error::error_code::_is_empty_iterator:
		return "is empty iterator";
	case json::value_error::error_code::_is_not_mutable_iterator:
		return "is not metable iterator";
	}
}


json_storage::json_storage() : _type (value_type::_null) {}

json_storage::json_storage(const json_storage & other){
	copy_data(other);
}

json_storage::json_storage(json_storage && other) {
	move_data(std::move(other));
}

json_storage & json_storage::operator=(const json_storage & other) {
	if (*this != other) {
		copy_data(other);
	}
	return *this;
}

json_storage & json_storage::operator=(json_storage && other) {
	if (*this != other) {
		move_data(std::move(other));
	}
	return *this;
}

bool json_storage::operator==(const json_storage & other) const {
	using vt = value_type;
	if (other._type != _type && !check_num_types(other.type(), _type)) {
		return false;
	}
	switch (_type) {
	case json::value_type::_null:
		return true;
	case json::value_type::_bool:
		return *other.get<bool>() == *this->get<bool>();
	case json::value_type::_int:
	case json::value_type::_uint:
	case json::value_type::_double:
		return compare_num_types(*this, other);
	case json::value_type::_string:
		return *other.get<std::string>() == *this->get<std::string>();
	case json::value_type::_array:
		return *other.get<json_array>() == *this->get<json_array>();
	case json::value_type::_object:
		return *other.get<json_object>() == *this->get<json_object>();
	}
}

bool json_storage::operator!=(const json_storage & other) const {
	return !(*this == other);
}

json_storage::~json_storage() {
	clear();
}

void json_storage::clear() noexcept {
	switch (_type) {
	case value_type::_string:
		get<std::string>()->~basic_string();
		break;
	case value_type::_array:
		get<json_array>()->~vector();
		break;
	case value_type::_object:
		get<json_object>()->~map();
		break;
	}
	_type = value_type::_null;
}

value_type json_storage::type() const{
	return _type;
}

void json_storage::type(value_type new_type){
	clear();
	switch (new_type) {
	case value_type::_bool:
		set<bool>(false);
		break;
	case value_type::_int:
		set<int>(0);
		break;
	case value_type::_uint:
		set<unsigned int>(0);
		break;
	case value_type::_double:
		set<double>(0);
		break;
	case value_type::_string:
		set<std::string>("");
		break;
	case value_type::_array:
		set<json_array>();
		break;
	case value_type::_object:
		set<json_object>();
		break;
	}
}

size_t json_storage::aligned() const noexcept {
	return align_size;
}

size_t json_storage::size() const noexcept {
	return buffer_size;
}

void json_storage::copy_data(const json_storage & other) {
	switch (other._type) {
	case value_type::_bool:
		set<bool>(*other.get<bool>());
		break;
	case value_type::_int:
		set<int>(*other.get<int>());
		break;
	case value_type::_uint:
		set<unsigned int>(*other.get<unsigned int>());
		break;
	case value_type::_double:
		set<double>(*other.get<double>());
		break;
	case value_type::_string:
		set<std::string>(*other.get<std::string>());
		break;
	case value_type::_array:
		set<json_array>(*other.get<json_array>());
		break;
	case value_type::_object:
		set<json_object>(*other.get<json_object>());
		break;
	}
}

void json_storage::move_data(json_storage && other) {
	switch (other._type) {
	case value_type::_bool:
		set<bool>(std::move(*other.get<bool>()));
		break;
	case value_type::_int:
		set<int>(std::move(*other.get<int>()));
		break;
	case value_type::_uint:
		set<unsigned int>(std::move(*other.get<unsigned int>()));
		break;
	case value_type::_double:
		set<double>(std::move(*other.get<double>()));
		break;
	case value_type::_string:
		set<std::string>(std::move(*other.get<std::string>()));
		other.get<std::string>()->~basic_string();
		break;
	case value_type::_array:
		set<json_array>(std::move(*other.get<json_array>()));
		other.get<json_array>()->~vector();
		break;
	case value_type::_object:
		set<json_object>(std::move(*other.get<json_object>()));
		other.get<json_object>()->~map();
		break;
	}
	other._type = value_type::_null;
}

bool json::core::impl::check_num_types(const value_type & a, const value_type & b) {
	return (a == value_type::_int || a == value_type::_double || a == value_type::_uint) 
		&& (b == value_type::_int || b == value_type::_double || b == value_type::_uint);
}

bool json::core::impl::compare_num_types(const json_storage & a, const json_storage & b){
	double var_a = cast_to_double(a);
	double var_b = cast_to_double(b);
	return var_a == var_b;
}

double json::core::impl::cast_to_double(const json_storage & data) {
	switch (data.type()) {
	case json::value_type::_int:
		return *data.get<int>();
	case json::value_type::_uint:
		return *data.get<unsigned int>();
	case json::value_type::_double:
		return *data.get<double>();
	}
	throw value_error(value_error::error_code::_type_is_not_numberic);
}

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
		throw value_error(value_error::error_code::_is_not_object_iterator);
	}
}

jv_reference json_value_iterator::value() {
	switch (_type) {
	case json_value_iterator::_iterator_type::_array:
		return *_data.arr_it;
	case json_value_iterator::_iterator_type::_object:
		return _data.object_iter->second;
	default:
		throw value_error(value_error::error_code::_is_not_mutable_iterator);
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
	throw value_error(value_error::error_code::_is_empty_iterator);
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
	throw value_error(value_error::error_code::_is_not_array_iterator);
}

json_value_iterator::const_array_iterator json_value_iterator::get_const_array_iterator() const {
	if (_type == _iterator_type::c_array) {
		return _data.const_arr_it;
	}
	throw value_error(value_error::error_code::_is_not_const_array_iterator);
}

json_value_iterator::object_iterator json_value_iterator::get_object_iterator() const {
	if (_type == _iterator_type::_object) {
		return _data.object_iter;
	}
	throw value_error(value_error::error_code::_is_not_object_iterator);
}

json_value_iterator::const_object_iterator json_value_iterator::get_const_object_iterator() const {
	if (_type == _iterator_type::c_object) {
		return _data.const_object_iter;
	}
	throw value_error(value_error::error_code::_is_not_const_object_iterator);
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

json::json_value::json_value() {
	_storage = std::make_unique<json_storage>();
}

json_value::json_value(value_type type) : json_value() {
	_storage->type(type);
}

json_value::json_value(bool data) : json_value() {
	_storage->set<bool>(data);
}

json::json_value::json_value(int data) : json_value() {
	_storage->set<int>(data);
}

json::json_value::json_value(unsigned int data) : json_value() {
	_storage->set<unsigned int>(data);
}

json_value::json_value(double data) : json_value() {
	_storage->set<double>(data);
}

json_value::json_value(const std::string & string) : json_value() {
	_storage->set<std::string>(string);
}

json_value::json_value(const char * string) : json_value() {
	_storage->set<std::string>(string);
}

json_value::json_value(json_array data) : json_value() {
	_storage->set<json_array>(data);
}

json_value::json_value(json_object data) : json_value() {
	_storage->set<json_object>(data);
}

json_value::json_value(const json_value & val) {
	_storage = std::make_unique<json_storage>(*val._storage);
}

json_value::json_value(json_value && val) {
	_storage = std::make_unique<json_storage>(std::move(*val._storage));
}

json_value::json_value(const char * name, const json_value & val) : json_value() {
	_storage->type(value_type::_object);
	_storage->get<json_object>()->insert({ name, val });
}

json_value::json_value(const char * name, json_value && val) : json_value() {
	_storage->type(value_type::_object);
	_storage->get<json_object>()->insert({ name, std::move(val) });
}

json_value & json_value::operator=(const json_value & val) {
	*_storage = *val._storage;
	return *this;
}

json_value & json_value::operator=(json_value && val) {
	*_storage = std::move(*val._storage);
	return *this;
}

json_value & json::json_value::operator=(bool & val){
	assign(val);
	return *this;
}

json_value & json::json_value::operator=(char c){
	assign(c);
	return *this;
}

json_value & json::json_value::operator=(unsigned int num) {
	assign(num);
	return *this;
}

json_value & json::json_value::operator=(int num){
	assign(num);
	return *this;
}

json_value & json::json_value::operator=(double num){
	assign(num);
	return *this;
}

json_value & json::json_value::operator=(const std::string & string){
	assign(string);
	return *this;
}

json_value & json::json_value::operator=(const char * string) {
	assign(string);
	return *this;
}

json_value & json::json_value::operator=(json_array array){
	assign(array);
	return *this;
}

json_value & json::json_value::operator=(json_object object){
	assign(object);
	return *this;
}

json_value::~json_value() {
	_storage->clear();
}

bool json::json_value::operator==(const json_value & jval) const {
	return *_storage == *jval._storage;
}

bool json::json_value::operator!=(const json_value & jval) const {
	return *_storage != *jval._storage;
}

bool & json_value::as_bool() {
	return *_storage->get<bool>();
}

bool & json_value::as_bool() const {
	return const_cast<json_value*>(this)->as_bool();
}

int & json::json_value::as_int() {
	return *_storage->get<int>();
}

int & json_value::as_int() const {
	return const_cast<json_value*>(this)->as_int();
}

unsigned int & json::json_value::as_uint() {
	return *_storage->get<unsigned int>();
}

unsigned int & json_value::as_uint() const {
	return const_cast<json_value*>(this)->as_uint();
}

double & json_value::as_double() {
	return *_storage->get<double>();
}

double & json_value::as_double() const {
	return const_cast<json_value*>(this)->as_double();
}

std::string & json_value::as_string() {
	return  *_storage->get<std::string>();
}

std::string & json_value::as_string() const {
	return const_cast<json_value*>(this)->as_string();
}

json_array * json_value::as_array() {
	return _storage->get<json_array>();
}

json_array * json_value::as_array() const {
	return const_cast<json_value*>(this)->as_array();
}

json_object * json_value::as_object() {
	return _storage->get<json_object>();
}

json_object * json_value::as_object() const {
	return const_cast<json_value*>(this)->as_object();
}

void json::json_value::assign(const json_value & jval){
	*_storage = *jval._storage;
}

void json::json_value::assign(json_value && jval){
	*_storage = std::move(*jval._storage);
}

void json::json_value::assign(bool val) {
	_storage->set<bool>(val);
}

void json::json_value::assign(char c) {
	_storage->set<std::string>(std::string(1, c));
}

void json::json_value::assign(int num) {
	_storage->set<int>(num);
}

void json::json_value::assign(unsigned int num) {
	_storage->set<unsigned int>(num);
}

void json::json_value::assign(double num) {
	_storage->set<double>(num);
}

void json::json_value::assign(const std::string & string) {
	_storage->set<std::string>(string);
}

void json::json_value::assign(const char * string) {
	_storage->set<std::string>(string);
}

void json::json_value::assign(json_array array){
	_storage->set<json_array>(array);
}

void json::json_value::assign(json_object object) {
	_storage->set<json_object>(object);
}

json_value & json::json_value::operator[](int index) {
	if (is_null()) {
		type(value_type::_array);
	}
	if (!is_array()) {
		throw value_error(value_error::error_code::_type_is_not_array);
	}
	if (index < 0) {
		throw value_error(value_error::error_code::_index_out_of_array_size);
	}
	json_array * arr = _storage->get<json_array>();
	if (index >= arr->size()) {
		arr->resize(index + 1);
	}
	return (*arr)[index];
}

const json_value & json::json_value::operator[](int index) const {
	if (!is_array()) {
		throw value_error(value_error::error_code::_type_is_not_array);
	}
	const json_array * arr = _storage->get<json_array>();
	if (index >= arr->size() || index < 0) {
		throw value_error(value_error::error_code::_index_out_of_array_size);
	}
	return (*arr)[index];
}

json_value & json::json_value::operator[](const char * key) {
	if (is_null()) {
		type(value_type::_object);
	}
	if (!is_object()) {
		throw value_error(value_error::error_code::_type_is_not_object);
	}
	json_object * obj = _storage->get<json_object>();
	return (*obj)[key];
}

const json_value & json::json_value::operator[](const char * key) const {
	if (!is_object()) {
		throw value_error(value_error::error_code::_type_is_not_object);
	}
	const json_object * obj = _storage->get<json_object>();
	auto res = obj->find(key);
	if (res == obj->end()) {
		throw value_error(value_error::error_code::_element_not_found);
	}
	return res->second;
}

json_value & json::json_value::operator[](const std::string & key) {
	return this->operator[](key.c_str());
}

const json_value & json::json_value::operator[](const std::string & key) const {
	return this->operator[](key.c_str());
}

json_value & json::json_value::at(int index){
	if (!is_array()) {
		throw value_error(value_error::error_code::_type_is_not_array);
	}
	json_array * arr = _storage->get<json_array>();
	if (index >= arr->size() || index < 0) {
		throw value_error(value_error::error_code::_index_out_of_array_size);
	}
	return (*arr)[index];
}

const json_value & json::json_value::at(int index) const {
	return const_cast<json_value*>(this)->at(index);
}

json_value & json::json_value::at(const char * key) {
	if (!is_object()) {
		throw value_error(value_error::error_code::_type_is_not_object);
	}
	json_object * obj = _storage->get<json_object>();
	auto res = obj->find(key);
	if (res == obj->end()) {
		throw value_error(value_error::error_code::_element_not_found);
	}
	return res->second;
}

const json_value & json::json_value::at(const char * key) const {
	if (!is_object()) {
		throw value_error(value_error::error_code::_type_is_not_object);
	}
	const json_object * obj = _storage->get<json_object>();
	auto res = obj->find(key);
	if (res == obj->end()) {
		throw value_error(value_error::error_code::_element_not_found);
	}
	return res->second;
}

json_value & json::json_value::at(const std::string & key) {
	return at(key);
}

const json_value & json::json_value::at(const std::string & key) const {
	return at(key);
}

jv_pointer json_value::find(const std::string & name) {
	return find(name.c_str());
}

jv_pointer json_value::find(const char * name) {
	jv_pointer result = nullptr;
	switch (_storage->type()) {
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
	switch (_storage->type()) {
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
	if (_storage->type() != value_type::_array) {
		throw value_error(value_error::error_code::_type_is_not_array);
	}
	jv_pointer result = nullptr;
	switch (_storage->type()) {
	case value_type::_array:
		_storage->get<json_array>()->push_back(val);
		result = &_storage->get<json_array>()->back();
		break;
	}
	return result;
}

jv_pointer json_value::add(json_value && val) {
	if (_storage->type() != value_type::_array) {
		throw value_error(value_error::error_code::_type_is_not_array);
	}
	jv_pointer result = nullptr;
	switch (_storage->type()) {
	case value_type::_array:
		_storage->get<json_array>()->push_back(std::move(val));
		result = &_storage->get<json_array>()->back();
		break;
	}
	return result;
}

jv_pointer json_value::add(const char * name, const json_value & val) {
	if (_storage->type() != value_type::_object && _storage->type() != value_type::_array) {
		throw value_error(value_error::error_code::_type_is_not_object_or_array);
	}
	jv_pointer result = nullptr;
	switch (_storage->type()) {
	case value_type::_object:
		_storage->get<json_object>()->insert_or_assign(name, val);
		result = &_storage->get<json_object>()->operator[](name);
		break;
	case value_type::_array:
		_storage->get<json_array>()->push_back(json_value(name, val));
		result = &_storage->get<json_array>()->back();
		break;
	}
	return result;
}

jv_pointer json_value::add(const char * name, json_value && val) {
	if (_storage->type() != value_type::_object && _storage->type() != value_type::_array) {
		throw value_error(value_error::error_code::_type_is_not_object_or_array);
	}
	jv_pointer result = nullptr;
	switch (_storage->type()) {
	case value_type::_object:
		_storage->get<json_object>()->insert_or_assign(name, std::move(val));
		result = &_storage->get<json_object>()->operator[](name);
		break;
	case value_type::_array:
		_storage->get<json_array>()->push_back(json_value(name, std::move(val)));
		result = &_storage->get<json_array>()->back();
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
	if (_storage->type() != value_type::_object) {
		throw value_error(value_error::error_code::_type_is_not_object);
	}
	auto target = _storage->get<json_object>()->find(key);
	if (target != _storage->get<json_object>()->end()) {
		_storage->get<json_object>()->erase(target);
	}
}

void json_value::remove(const std::string & key){
	remove(key.c_str());
}

void json_value::remove(size_t num){
	if (_storage->type() != value_type::_array || num < 0) {
		throw value_error(value_error::error_code::_type_is_not_array);
	}
	if (num > _storage->get<json_array>()->size() || num < 1) {
		throw value_error(value_error::error_code::_index_out_of_array_size);
	}
	_storage->get<json_array>()->erase(_storage->get<json_array>()->begin() + num);
}

void json_value::clear(){
	_storage->clear();
}

json_value json_value::extract(const char * key){
	if (_storage->type() != value_type::_object) {
		throw value_error(value_error::error_code::_type_is_not_object);
	}
	auto target = _storage->get<json_object>()->find(key);
	json_value res = std::move(target->second);
	if (target != _storage->get<json_object>()->end()) {
		_storage->get<json_object>()->erase(target);
	}
	return res;
}

json_value json_value::extract(const std::string & key){
	return extract(key.c_str());
}

json_value json::json_value::extract(int index) {
	if (_storage->type() != value_type::_array) {
		throw value_error(value_error::error_code::_type_is_not_array);
	}
	auto arr = as_array();
	if (index >= arr->size() || index < 0) {
		throw value_error(value_error::error_code::_index_out_of_array_size);
	}
	auto target = std::move(arr->operator[](index));
	arr->erase(arr->begin() + index);
	return target;
}

value_type json_value::type() const {
	return _storage->type();
}

void json_value::type(value_type _t) {
	if (_t != _storage->type()) {
		_storage->type(_t);
	}
}

bool json_value::is_null() const {
	return _storage->type() == value_type::_null;
}

bool json_value::is_bool() const {
	return _storage->type() == value_type::_bool;
}

bool json::json_value::is_int() const{
	return _storage->type() == value_type::_int;
}

bool json::json_value::is_uint() const {
	return _storage->type() == value_type::_uint;
}

bool json_value::is_double() const {
	return _storage->type() == value_type::_double;
}

bool json_value::is_string() const {
	return _storage->type() == value_type::_string;
}

bool json_value::is_array() const {
	return _storage->type() == value_type::_array;
}

bool json_value::is_object() const {
	return _storage->type() == value_type::_object;
}

size_t json_value::item_count(){
	size_t count = 0;
	switch (_storage->type()){
	case value_type::_null:
	case value_type::_int:
	case value_type::_uint:
	case value_type::_double:
	case value_type::_bool:
	case value_type::_string:
		count =  1;
		break;
	case value_type::_array:
		// count += 1;
		for (auto it = _storage->get<json_array>()->begin(); it != _storage->get<json_array>()->end(); ++it) {
			count += it->item_count();
			if (it->type() == value_type::_object) {
				count++;
			}

		}
		break;
	case value_type::_object:
	//	count += 1;
//		count += _storage->get<json_object>()->size();
		for (auto it = _storage->get<json_object>()->begin(); it != _storage->get<json_object>()->end(); ++it) {
			count += it->second.item_count();
			if (it->second.type() == value_type::_object) {
				count++;
			}
		}
		break;
	}
	return count;
}

size_t json::json_value::size() const noexcept {
	switch (_storage->type()) {
	case json::value_type::_null:
		return 0;
	case json::value_type::_string:
		return _storage->get<std::string>()->size();
	case json::value_type::_array:
		return _storage->get<json_array>()->size();
	case json::value_type::_object:
		return _storage->get<json_object>()->size();
	}
	return 1;
}

bool json::json_value::empty() const noexcept {
	switch (_storage->type()) {
	case json::value_type::_null:
		return true;
	case json::value_type::_string:
		return _storage->get<std::string>()->empty();
	case json::value_type::_array:
		return _storage->get<json_array>()->empty();
	case json::value_type::_object:
		return _storage->get<json_object>()->empty();
	}
	return false;
}

json_value_iterator json_value::begin(){
	switch (_storage->type()){
	case value_type::_array:
		return json_value_iterator(_storage->get<json_array>()->begin());
	case value_type::_object:
		return json_value_iterator(_storage->get<json_object>()->begin());
	default:
		throw value_error(value_error::error_code::_type_is_not_object_or_array);
	}
}

json_value_iterator json_value::begin() const {
	return cbegin();
}

json_value_iterator json_value::cbegin() const{
	switch (_storage->type()) {
	case value_type::_array:
		return json_value_iterator(_storage->get<json_array>()->cbegin());
	case value_type::_object:
		return json_value_iterator(_storage->get<json_object>()->cbegin());
	default:
		throw value_error(value_error::error_code::_type_is_not_object_or_array);
	}
}

json_value_iterator json_value::end(){
	switch (_storage->type()) {
	case value_type::_array:
		return json_value_iterator(_storage->get<json_array>()->end());
	case value_type::_object:
		return json_value_iterator(_storage->get<json_object>()->end());
	default:
		throw value_error(value_error::error_code::_type_is_not_object_or_array);
	}
}

json_value_iterator json_value::end() const{
	return cend();
}

json_value_iterator json_value::cend() const {
	switch (_storage->type()) {
	case value_type::_array:
		return json_value_iterator(_storage->get<json_array>()->cend());
	case value_type::_object:
		return json_value_iterator(_storage->get<json_object>()->cend());
	default:
		throw value_error(value_error::error_code::_type_is_not_object_or_array);
	}
}

jv_pointer json_value::find_impl(json_value & val, const char * name){
	switch (val._storage->type()) {
	case value_type::_array:
		return val.find_in_array(name);
	case value_type::_object:
		return val.find_in_object(name);
	}
	return nullptr;
}

jv_pointer json_value::find_in_array(const char * name) {
	jv_pointer _result = nullptr;
	for (size_t _i = 0; _i < _storage->get<json_array>()->size(); ++_i) {
		_result = find_impl(_storage->get<json_array>()->operator[](_i), name);
		if (_result) {
			break;
		}
	}
	return _result;
}

jv_pointer json_value::find_in_object(const char * name) {
	jv_pointer _result = nullptr;
	if (_storage->get<json_object>()->count(std::string(name)) != 0) {
		_result = &_storage->get<json_object>()->operator[](name);
	}
	if (!_result) {
		for (auto it = _storage->get<json_object>()->begin(); it != _storage->get<json_object>()->end(); ++it) {
			_result = find_impl(it->second, name);
			if (_result) {
				break;
			}
		}
	}
	return _result;
}

void json_value::select_impl(json_pointer_array & res, json_pointer_array & sub, json_value & val, const char * name){
	switch (val._storage->type()) {
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
	for (size_t _i = 0; _i < _storage->get<json_array>()->size(); ++_i) {
		select_impl(_result, _sub_result, _storage->get<json_array>()->operator[](_i), name);
	}
	return _result;
}

json_pointer_array json_value::select_in_object(const char * name){
	json_pointer_array _result;
	json_pointer_array _sub_result;
	if (_storage->get<json_object>()->count(std::string(name)) != 0) {
		_result.push_back(&_storage->get<json_object>()->operator[](name));
	}
	for (auto it = _storage->get<json_object>()->begin(); it != _storage->get<json_object>()->end(); ++it) {
		select_impl(_result, _sub_result, it->second, name);
	}
	return _result;
}