#include "json.h"

json::json_doc::json_doc() : _root(nullptr) { }

json::json_doc::json_doc(const json_value & val){
	_root = new json_value(val);
}

json::json_doc::json_doc(json_value && val){
	_root = new json_value(std::move(val));
}

json::json_doc::json_doc(const json_doc & doc)
{
}

json::json_doc::json_doc(json_doc && doc)
{
}

json::json_doc::~json_doc(){
	if (_root) {
		delete _root;
	}
}

json::json_doc & json::json_doc::operator=(const json_doc & doc)
{
	// TODO: вставьте здесь оператор return
}

json::json_doc & json::json_doc::operator=(json_doc && doc)
{
	// TODO: вставьте здесь оператор return
}

void json::json_doc::load(const char * data_string, bool is_file){
	if (!_errors.empty()) {
		_errors.clear();
	}
	if (is_file) {
		_root = new json_value(_parser.load_from_file(data_string));
	}
	else {
		_root = new json_value(_parser.load_from_string(data_string));
	}
	if (_parser.get_last_error().type != error_type::_none) {
		_errors.push_back(_parser.get_last_error());
	}
}

void json::json_doc::load(std::istream & stream){
	if (!_errors.empty()) {
		_errors.clear();
	}
	_root = new json_value(_parser.load_from_stream(stream));
	if (_parser.get_last_error().type != error_type::_none) {
		_errors.push_back(_parser.get_last_error());
	}
}

void json::json_doc::save(const char * data_string){
	if (!_errors.empty()) {
		return;
	}
	_writer.write_to_file(*_root, data_string);
	if (_parser.get_last_error().type != error_type::_none) {
		_errors.push_back(_parser.get_last_error());
	}
}

void json::json_doc::save(std::ostream & stream){
	if (!_errors.empty()) {
		return;
	}
	_writer.write_to_stream(*_root, stream);
	if (_parser.get_last_error().type != error_type::_none) {
		_errors.push_back(_parser.get_last_error());
	}
}

std::string json::json_doc::to_string(){
	if (!_errors.empty()) {
		return;
	}
	std::string result;
	_writer.write_to_string(*_root, result);
	return result;
}

json::error json::json_doc::get_last_error(){
	return _errors.back();
}

json::json_value * json::json_doc::root(){
	return _root;
}

bool json::json_doc::has(const char * key){
	return _root->find(key) != nullptr;
}

json::json_value * json::json_doc::get(const char * key){
	json_value * res = _root->find(key);
	return res;
}
