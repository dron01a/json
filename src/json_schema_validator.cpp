#include "json_schema_validator.h"

using namespace json;

json_schema_validator::json_schema_validator(const json_value & scheme, size_t max_errors) : 
	_scheme(scheme), 
	_max_errors_count(max_errors) {}

bool json_schema_validator::validate(const json_value & inst) {
	push_path("/");
	_errors.clear();
	validate_value(_scheme, inst);
	if (_errors.empty()) {
		return true;
	}
	return false;
}

std::vector<validation_error> json::json_schema_validator::errors() {
	return _errors;
}

void json_schema_validator::validate_value(const json_value & scheme, const json_value & value) {
	if (scheme.contains("type")) {
		validate_type(scheme["type"], value);
	}
	if (scheme.contains("enum")) {
		validate_enum(scheme["enum"], value);
	}
	if (scheme.contains("const")) {
		validate_const(scheme["const"], value);
	}
	if (value.is_int() || value.is_uint() || value.is_double()) {
		validate_number(scheme, value);
	}
	if (value.is_string()) {
		validate_string(scheme, value);
	}
	if (value.is_array()) {
		validate_array(scheme, value);
	}
	if (value.is_object()) {
		validate_object(scheme, value);
	}
}

void json_schema_validator::validate_type(const json_value & scheme, const json_value & value) {
	std::string type = type_to_string(value.type()); // ïîëó÷àåì èìÿ òèïà
	switch (scheme.type()) {
	case value_type::_string:
		if (type != scheme.as_string()) {
			add_error(validation_error_code::incorrect_type);
		}
		break;
	case value_type::_array:
		for (const auto& t : *scheme.as_array()) {
			if (type == t.as_string()) {
				return;
			}
		}
		add_error(validation_error_code::incorrect_type);
		break;
	}
}

void json::json_schema_validator::validate_enum(const json_value & scheme, const json_value & value) {
	if (!scheme.is_array()) {
		return;
	}
	for (const auto& data : *scheme.as_array()) {
		if (value == data) {
			return;
		}
	}
	add_error(validation_error_code::no_matches_enum);
}

void json::json_schema_validator::validate_const(const json_value & scheme, const json_value & value) {
	if (scheme != value) {
		add_error(validation_error_code::no_matches_const);
	}
}

void json::json_schema_validator::validate_number(const json_value & scheme, const json_value & value) {
	double data = jv_to_d(value);
	if (scheme.contains("minimum")) {
		if (data < jv_to_d(scheme["minimum"])) {
			add_error(validation_error_code::value_is_less_as_minimum);
		}
	}
	if (scheme.contains("maximum")) {
		if (data > jv_to_d(scheme["maximum"])) {
			add_error(validation_error_code::value_is_greater_as_maximum);
		}
	}
	if (scheme.contains("exclusiveMinimum")) {
		if (data <= jv_to_d(scheme["exclusiveMinimum"])) {
			add_error(validation_error_code::value_is_less_as_minimum);
		}
	}
	if (scheme.contains("exclusiveMaximum")) {
		if (data >= jv_to_d(scheme["exclusiveMaximum"])) {
			add_error(validation_error_code::value_is_greater_as_maximum);
		}
	}
	if (scheme.contains("multipleOf")) {
		if ((int)data % (int)jv_to_d(scheme["multipleOf"]) != 0) {
			add_error(validation_error_code::not_multiple_of);
		}
	}
}

void json::json_schema_validator::validate_string(const json_value & scheme, const json_value & value) {
	std::string data = value.as_string();
	if (scheme.contains("minLength")) {
		if (scheme["minLength"].is_number()) {
			if (data.length() < jv_to_d(scheme["minLength"])) {
				add_error(validation_error_code::str_len_is_less_as_minimum);
			}
		}
	}
	if (scheme.contains("maxLength")) {
		if (scheme["minLength"].is_number()) {
			if (data.length() > jv_to_d(scheme["maxLength"])) {
				add_error(validation_error_code::str_len_is_greater_as_maximum);
			}
		}
	}
	if (scheme.contains("pattern")) {
		if (scheme["pattern"].is_string()) {
			std::regex re(scheme["pattern"].as_string());
			if (!std::regex_match(data, re)) {
				add_error(validation_error_code::str_no_pattern);
			}
		}
	}
	if (scheme.contains("format")) {
		validate_format(scheme["format"], value);
	}
}

void json::json_schema_validator::validate_format(const json_value & scheme, const json_value & value) {
	if (!scheme.is_string()) {
		return;
	}
	std::string format = scheme.as_string();
	if (format == "date-time") {
		check_regex(value.as_string(), 
					R"(^(\d{4})-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])T([01]\d|2[0-3]):([0-5]\d):([0-5]\d)(\.\d+)?(Z|[+-](0[1-9]|1[0-2]):[0-5]\d)?$)",
			        validation_error_code::not_match_format_data_time);
	} 
	else if (format == "date") {
		check_regex(value.as_string(), 
			        R"(^(\d{4})-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$)", 
			        validation_error_code::not_match_format_data);
	}
	else if (format == "time") {
		check_regex(value.as_string(), 
			        R"(^\d{2}:\d{2}:\d{2}(.\d+)?(Z|[+-]\d{2}:\d{2})?$)",
					validation_error_code::not_match_format_time);
	}
	else if (format == "email") {
		check_regex(value.as_string(), R"(^[-\w]+@([A-z0-9][-A-z0-9]+\.)+[A-z]{2,4}$)", validation_error_code::not_match_format_time);
	}
	else if (format == "hostname") {
		check_regex(value.as_string(), 
			        R"(^([a-zA-Z0-9]([a-zA-Z0-9\-]{0,61}[a-zA-Z0-9])?\.)+[a-zA-Z]{2,6}$)",
			        validation_error_code::not_match_format_time);
	}
	else if (format == "ipv4") {
		check_regex(value.as_string(),
			R"(^((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$)",
			validation_error_code::not_match_format_time);
	}
	else if (format == "ipv6") {
		check_regex(value.as_string(),
			R"(((^|:)([0-9a-fA-F]{0,4})){1,8}$)",
			validation_error_code::not_match_format_time);
	}
	else if (format == "uri") {
		check_regex(value.as_string(),
			R"(^(https?:\/\/)([a-zA-Zà-ÿÀ-ß¨¸0-9-]+\.)+[a-zA-Zà-ÿÀ-ß¨¸]{2,}(\/[a-zA-Zà-ÿÀ-ß¨¸0-9-_~%.]*)*\/?$)",
			validation_error_code::not_match_format_time);
	}
	else if (format == "uuid") {
		check_regex(value.as_string(),
			R"(^[0-9A-Fa-f]{8}\-[0-9A-Fa-f]{4}\-[0-9A-Fa-f]{4}\-[0-9A-Fa-f]{4}\-[0-9A-Fa-f]{12}$)",
			validation_error_code::not_match_format_time);
	}
}

void json::json_schema_validator::validate_array(const json_value & scheme, const json_value & value) {
	const auto & arr = value.as_array();
	if (scheme.contains("minItems")) {
		if (arr->size() < jv_to_d(scheme["minItems"])) {
			add_error(validation_error_code::array_size_less_as_minimum);
		}
	}
	if (scheme.contains("maxItems")) {
		if (arr->size() > jv_to_d(scheme["maxItems"])) {
			add_error(validation_error_code::array_size_greater_as_maximum);
		}
	}
	if (scheme.contains("items")) {
		array_items_handler(scheme["items"], *arr);
	}
	if (scheme.contains("additionalItems")) {
		array_additionalItems_handler(scheme.find("items"), scheme["additionalItems"], *arr);
	}
	if (scheme.contains("uniqueItems")) {
		array_uniqueItems_handler(scheme["uniqueItems"], *arr);
	}
	if (scheme.contains("contains")) {
		array_contains_handler(scheme["contains"], *arr);
	}
}

void json::json_schema_validator::validate_object(const json_value & scheme, const json_value & value) {
	const auto & obj = value.as_object();
	if (scheme.contains("propertyNames")) {
		object_propertyNames_handler(scheme["propertyNames"], *obj);
	}
	if (scheme.contains("minProperties")) {
		if (obj->size() < jv_to_d(scheme["minProperties"])) {
			add_error(validation_error_code::object_properties_less_as_minimum);
		}
	}
	if (scheme.contains("maxProperties")) {
		if (obj->size() > jv_to_d(scheme["maxProperties"])) {
			add_error(validation_error_code::object_properties_greater_as_maximum);
		}
	}
	if (scheme.contains("properties")) {
		object_properties_handler(scheme["properties"], *obj);
		if (scheme.contains("additionalProperties")) {
			object_properties_additional_handler(scheme["additionalProperties"], scheme["properties"], *obj);
		}
	}
	if (scheme.contains("patternProperties")) {
		object_patternProperties_handler(scheme["patternProperties"], *obj);
		if (scheme.contains("additionalProperties")) {
			object_patternProperties_additional_handler(scheme["additionalProperties"], scheme["patternProperties"], *obj);
		}
	}
	if (scheme.contains("required")) {
		object_required_handler(scheme["required"], *obj);
	}
	if (scheme.contains("dependencies")) {
		object_dependencies_handler(scheme["dependencies"], *obj);
	}
}

void json::json_schema_validator::array_items_handler(const json_value & items_scheme, json_array & arr){
	switch (items_scheme.type()) {
	case value_type::_object:
		for (auto it = arr.begin(); it != arr.end(); ++it) {
			validate_value(items_scheme, *it);
		}
		break;
	case value_type::_array:
		if (arr.size() < items_scheme.size()) {
			add_error(validation_error_code::array_size_no_match);
		}
		else {
			for (size_t i = 0; i < items_scheme.size(); ++i) {
				validate_value(items_scheme[i], arr[i]);
			}
		}
		break;
	}
}

void json::json_schema_validator::array_uniqueItems_handler(const json_value & field, json_array & arr){
	if (!field.is_bool()) {
		return;
	}
	if (!field.as_bool() || arr.size() <= 1) {
		return;
	}
	for (size_t i = 1; i < arr.size(); ++i) {
		if (arr[i] != arr[i - 1]) {
			add_error(validation_error_code::array_items_is_not_unique);
			break;
		}
	}
}

void json::json_schema_validator::array_additionalItems_handler(jv_pointer items, const json_value & field, json_array & arr) {
	if (!items || arr.size() < items->size()) {
		return;
	}
	if (field.is_bool()) {
		if (!field.as_bool() && arr.size() > items->size()) {
			add_error(validation_error_code::array_size_no_match);
		}
	}
	else if (field.as_object()) {
		for (size_t i = items->size(); i < arr.size(); ++i) {
			validate_value(field, arr[i]);
		}
	}
}

void json::json_schema_validator::array_contains_handler(const json_value & field, json_array & arr) {
	json_schema_validator sub_val(field);
	for (size_t i = 0; i < arr.size(); ++i) {
		if (sub_val.validate(arr[i])) {
			return;
		}
	}
	add_error(validation_error_code::array_havent_contains_item);
}

void json::json_schema_validator::object_propertyNames_handler(const json_value & field, json_object & obj) {
	for (auto it = obj.begin(); it != obj.end(); ++it) {
		validate_string(field, it->first);
	}
}

void json::json_schema_validator::object_properties_handler(const json_value & field, json_object & obj) {
	if (!field.is_object()) {
		return;
	}
	auto field_obj = field.as_object();
	for (auto it = field_obj->begin(); it != field_obj->end(); ++it) {
		if (obj.count(it->first) == 0) {
			add_error(validation_error_code::object_no_have_property);
			break;
		}
		validate_value(it->second, obj[it->first]);
	}
}

void json::json_schema_validator::object_patternProperties_handler(const json_value & field, json_object & obj) {
	if (!field.is_object()) {
		return;
	}
	auto field_obj = field.as_object();
	for (auto obj_it = obj.begin(); obj_it != obj.end(); ++obj_it) {
		for (auto field_it = field_obj->begin(); field_it != field_obj->end(); ++field_it) {
			std::regex cur_regex(field_it->first);
			if (std::regex_match(obj_it->first, cur_regex)) {
				validate_value(field_it->second, obj_it->second);
			}
		}
	}
}

void json::json_schema_validator::object_properties_additional_handler(const json_value & field, const json_value & properties, json_object & obj) {
	if (field.is_bool()) {
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			if (!field.as_bool() && !properties.contains(it->first)) {
				add_error(validation_error_code::object_additional_properties);
				break;
			}
		}
	}
	else if (field.is_object()) {
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			if (!properties.contains(it->first)) {
				validate_value(field, it->second);
			}
		}
	}
}

void json::json_schema_validator::object_patternProperties_additional_handler(const json_value & field, const json_value & patt_prop, json_object & obj) {
	auto patternProperties_obj = patt_prop.as_object();
	if (field.is_bool()) {
		for (auto it_obj = obj.begin(); it_obj != obj.end(); ++it_obj) {
			for (auto it_pp = patternProperties_obj->begin(); it_pp != patternProperties_obj->end(); ++it_pp) {
				std::regex cur_regex(it_pp->first);
				if (!field.as_bool() && !std::regex_match(it_obj->first, cur_regex)) {
					add_error(validation_error_code::object_additional_properties);
					return;
				}
			}
		}
	}
	else if (field.is_object()) {
		for (auto it_obj = obj.begin(); it_obj != obj.end(); ++it_obj) {
			for (auto it_pp = patternProperties_obj->begin(); it_pp != patternProperties_obj->end(); ++it_pp) {
				std::regex cur_regex(it_pp->first);
				if (!std::regex_match(it_obj->first, cur_regex)) {
					validate_value(field, it_obj->second);
				}
			}
		}
	}
}

void json::json_schema_validator::object_required_handler(const json_value & field, json_object & obj) {
	if (!field.is_array()) {
		return;
	}
	const auto required = field.as_array();
	for (auto r : *required) {
		if (!r.is_string()) {
			continue;
		}
		if (obj.count(r.as_string()) == 0) {
			add_error(validation_error_code::object_required);
			continue;
		}
	}
}

void json::json_schema_validator::object_dependencies_handler(const json_value & field, json_object & obj) {
	if (!field.is_object()) {
		return;
	}
	auto dependencies = field.as_object();
	for (auto it = dependencies->begin(); it != dependencies->end(); ++it) {
		if (obj.count(it->first) == 0) {
			add_error(validation_error_code::object_dependencies);
			continue;
		}
		if (!it->second.is_array()) {
			continue;
		}
		auto & sub_value = obj[it->first];
		auto dep_array = it->second.as_array();
		for (auto sub_it = dep_array->begin(); sub_it != dep_array->end(); ++sub_it) {
			if (!sub_it->is_string()) {
				continue;
			}
			if (!sub_value.contains(sub_it->as_string())) {
				add_error(validation_error_code::object_dependencies);
			}
		}
	}
}

std::string json::json_schema_validator::type_to_string(value_type type) {
	switch (type) {
	case json::value_type::_null:
		return "null";
	case json::value_type::_bool:
		return "boolean";
	case json::value_type::_int:
		return "number";
	case json::value_type::_uint:
		return "number";
	case json::value_type::_double:
		return "number";
	case json::value_type::_string:
		return "string";
	case json::value_type::_array:
		return "array";
	case json::value_type::_object:
		return "object";
	}
}

void json::json_schema_validator::push_path(std::string name) {
	if (cur_path.empty()) {
		cur_path += '/';
	}
	else if (name[0] == '[') {
		cur_path += name;
	}
	else {
		cur_path += '/' + name;
	}
}

void json::json_schema_validator::pop_path() {
	size_t pos = cur_path.find_first_not_of('/');
	if (pos == std::string::npos) {
		cur_path.clear();
	}
	else {
		cur_path = cur_path.substr(0, pos);
	}
}

void json::json_schema_validator::add_error(validation_error_code err) {
	if (_errors.size() >= _max_errors_count) {
		throw valid_end();
	}
	_errors.push_back(validation_error(err, cur_path));
}

double json::json_schema_validator::jv_to_d(const json_value & value) {
	if (value.is_double()) {
		return value.as_double();
	}
	if (value.is_uint()) {
		return value.as_uint();
	}
	if (value.is_int()) {
		return value.as_int();
	}
	return 0;
}

void json_schema_validator::check_regex(const std::string & str, const std::string & regex_str, validation_error_code code) {
	std::regex regex(regex_str);
	if (!std::regex_match(str, regex)) {
		add_error(code);
	}
}
