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
	std::string type = type_to_string(value.type()); // получаем им€ типа
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
		if(data.length() < jv_to_d(scheme["minLength"])){
			add_error(validation_error_code::str_len_is_less_as_minimum);
		}
	}
	if (scheme.contains("maxLength")) {
		if (data.length() > jv_to_d(scheme["maxLength"])) {
			add_error(validation_error_code::str_len_is_greater_as_maximum);
		}
	}
	if (scheme.contains("pattern")) {
		if (!scheme["pattern"].is_string()) {
			// to-do бросок ошибки
		}
		std::regex re(scheme["pattern"].as_string());
		if (!std::regex_match(data, re)) {
			add_error(validation_error_code::str_no_pattern);
		}
	}
	if (scheme.contains("format")) {
		if (!scheme["format"].is_string()) {
			// to-do бросок ошибки
		}
		validate_format(scheme["format"], value);
	}
}

void json::json_schema_validator::validate_format(const json_value & scheme, const json_value & value) {
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
			R"(^(https?:\/\/)([a-zA-Zа-€ј-я®Є0-9-]+\.)+[a-zA-Zа-€ј-я®Є]{2,}(\/[a-zA-Zа-€ј-я®Є0-9-_~%.]*)*\/?$)",
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
	const auto items = scheme.find("items");
	if (items) {
		switch (items->type()) {
		case value_type::_object:
			for (auto it = arr->begin(); it != arr->end(); ++it) {
				validate_value(*items, *it);
			}
			break;
		case value_type::_array:
			if (arr->size() < items->size()) {
				add_error(validation_error_code::array_size_no_match);
			}
			else {
				for (size_t i = 0; i < items->size(); ++i) {
					validate_value(items->operator[](i), arr->operator[](i));
				}
			}
			break;
		}
	}
	if (scheme.contains("additionalItems")) {
		if (scheme["additionalItems"].is_bool()) {
			if (!scheme["additionalItems"].as_bool() && arr->size() > items->size()) {
				add_error(validation_error_code::array_size_no_match);	
			}
		}
	}
	if (scheme.contains("uniqueItems")) {
		if (scheme["uniqueItems"].is_bool()) {
			if (scheme["uniqueItems"].as_bool() && arr->size() > 1) {
				for (size_t i = 1; i < arr->size(); ++i) {
					if (arr->operator[](i) != arr->operator[](i-1)) {
						add_error(validation_error_code::array_items_is_not_unique);
						break;
					}
				}
			}
		}
	}
	if (scheme.contains("contains")) {
		bool _res = false;
		json_schema_validator sub_val(scheme["contains"]);
		for (size_t i = 0; i < arr->size(); ++i) {
			if (sub_val.validate(arr->operator[](i))) {
				_res = true;
			}
		}
		if (!_res) {
			add_error(validation_error_code::array_havent_contains_item);
		}
	}
}

void json::json_schema_validator::validate_object(const json_value & scheme, const json_value & value) {
	const auto & obj = value.as_object();
	if (scheme.contains("propertyNames")) {
		for (auto it = obj->begin(); it != obj->end(); ++it) {
			validate_string(_scheme["propertyNames"], it->first);
		}
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
		if (scheme["properties"].is_object()) {
			auto obj = scheme["properties"].as_object();
			for (auto it = obj->begin(); it != obj->end(); ++it) {
				if (!value.contains(it->first)) {
					add_error(validation_error_code::object_no_have_property);
					break;
				}
				validate_value(it->second, value[it->first]);
			}
		}
	}
	if (scheme.contains("patternProperties")) {
		if (scheme["patternProperties"].is_object()) {
			auto pp_obj = scheme["patternProperties"].as_object();
			for (auto it_obj = obj->begin(); it_obj != obj->end(); ++it_obj) {
			//	bool res = false;
				for (auto it_pp = pp_obj->begin(); it_pp != pp_obj->end(); ++it_pp) {
					std::regex cur_regex(it_pp->first);
					if (std::regex_match(it_obj->first, cur_regex)) {
						validate_value(it_pp->second, it_obj->second);
					}
				}
			/*	if (!res) {
					add_error(validation_error_code::object_additional_properties);
					break;
				}*/
			}
		}
	}
	if (scheme.contains("additionalProperties")) {
		bool check_properties = false; // флаг проверки полей в properties
		auto prop = scheme.find("properties");
		if (prop) {
			check_properties = prop->is_object();
		}
		bool check_patternProperties = false; // флаг проверки полей в patternProperties
		auto patt_prop = scheme.find("patternProperties");
		if (check_patternProperties) {
			check_patternProperties = patt_prop->is_object();
		}
		if (check_properties) {
			auto obj = value.as_object();
			if (scheme["additionalProperties"].is_bool()) {
				for (auto it = obj->begin(); it != obj->end(); ++it) {
					if (!scheme["additionalProperties"].as_bool() && !prop->contains(it->first)) {
						add_error(validation_error_code::object_additional_properties);
						break;
					}
				}
			}
			else if (scheme["additionalProperties"].is_object()) {
				for (auto it = obj->begin(); it != obj->end(); ++it) {
					if (!prop->contains(it->first)) {
						validate_value(scheme["additionalProperties"], it->second);
					}
				}
			}
		}
		if (check_patternProperties) {
			auto obj = value.as_object();
			auto pp_obj = patt_prop->as_object();
			if (scheme["additionalProperties"].is_bool()) {
				for (auto it_obj = obj->begin(); it_obj != obj->end(); ++it_obj) {
					bool res = false;
					for (auto it_pp = pp_obj->begin(); it_pp != pp_obj->end(); ++it_pp) {
						std::regex cur_regex(it_pp->first);
						if (!scheme["additionalProperties"].as_bool() && std::regex_match(it_obj->first, cur_regex)) {
							res = true;
							break;
						}
					}
					if (!res) {
						add_error(validation_error_code::object_additional_properties);
						break;
					}
				}
			}
			else if (scheme["additionalProperties"].is_object()) {
				for (auto it_obj = obj->begin(); it_obj != obj->end(); ++it_obj) {
					bool res = false;
					for (auto pp_obj = obj->begin(); pp_obj != obj->end(); ++pp_obj) {
						std::regex cur_regex(pp_obj->first);
						if (std::regex_match(it_obj->first, cur_regex)) {
							res = true;
							break;
						}
					}
					if (!res) {
						validate_value(scheme["additionalProperties"], it_obj->second);
					}
				}
			}
		}
	}
	if (scheme.contains("required")) {
		if (scheme["required"].is_array()) {
			auto required = scheme["required"].as_array();
			bool res = true;
			for (auto r : *required) {
				if (!r.is_string()) {
					continue;
				}
				if (!value.contains(r.as_string())) {
					res = false;
					break;
				}
			}
			if (!res) {
				add_error(validation_error_code::object_required);
			}
		}
	}
	if (scheme.contains("dependencies")) {
		auto dependencies = scheme["dependencies"].as_object();
		for (auto it = dependencies->begin(); it != dependencies->end(); ++it) {
			if (!value.contains(it->first)) {
				add_error(validation_error_code::object_dependencies);
			}
			auto & sub_value = value[it->first];
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
	throw; // to-do добавить бросок ошибок
}

void json_schema_validator::check_regex(const std::string & str, const std::string & regex_str, validation_error_code code) {
	std::regex regex(regex_str);
	if (!std::regex_match(str, regex)) {
		add_error(code);
	}
}
