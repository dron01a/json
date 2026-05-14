#include <iostream>
#include <cassert>

#include "json.h"
#include "json_schema_validator.h"

#include "../tools/tests_framework.h"

using namespace json;
using namespace json::core::impl;

TEST_CASE(validate_number) {
	auto schema1 = dom_parser::from_string("{\"type\" : \"number\", \"minimum\" : 2, \"maximum\" : 4 }").json_val;
	json_schema_validator validator1(schema1);
	auto value1 = dom_parser::from_string("3").json_val;
	auto value2 = dom_parser::from_string("3.5").json_val;
	auto value3 = dom_parser::from_string("-3").json_val;
	auto value4 = dom_parser::from_string("0").json_val;
	TEST_ASSERT(validator1.validate(value1));
	TEST_ASSERT(validator1.validate(value2));
	TEST_ASSERT(!validator1.validate(value3));
	TEST_ASSERT(!validator1.validate(value4));
	auto schema2 = dom_parser::from_string("{\"type\" : \"number\", \"multipleOf\" : 2 }").json_val;
	json_schema_validator validator2(schema2);
	auto value5 = dom_parser::from_string("10").json_val;
	auto value6 = dom_parser::from_string("5").json_val;
	TEST_ASSERT(validator2.validate(value5));
	TEST_ASSERT(!validator2.validate(value6));
}

TEST_CASE(validate_string) {
	auto schema1 = dom_parser::from_string("{\"type\" : \"string\", \"minLength\" : 2, \"maxLength\" : 6 }").json_val;
	json_schema_validator validator1(schema1);
	auto value1 = dom_parser::from_string("\"stroka\"").json_val;
	auto value2 = dom_parser::from_string("\"big stroka\"").json_val;
	TEST_ASSERT(validator1.validate(value1));
	TEST_ASSERT(!validator1.validate(value2));
	auto schema2 = dom_parser::from_string("{\"type\" : \"string\", \"pattern\" : \"^[a-z]+$\" }").json_val;
	json_schema_validator validator2(schema2);
	auto value3 = dom_parser::from_string("\"stroka\"").json_val;
	auto value4 = dom_parser::from_string("\"StRoka\"").json_val;
	TEST_ASSERT(validator2.validate(value3));
	TEST_ASSERT(!validator2.validate(value4));
}

TEST_CASE(validate_date_time_string_format) {
	auto data_time_schema = dom_parser::from_string("{\"type\" : \"string\", \"format\" : \"date-time\" }").json_val;
	json_schema_validator data_time_validator(data_time_schema);
	auto value1 = dom_parser::from_string("\"2001-12-22T17:00:00Z\"").json_val;
	auto value2 = dom_parser::from_string("\"20-45-9665T12:115\"").json_val;
	TEST_ASSERT(data_time_validator.validate(value1));
	TEST_ASSERT(!data_time_validator.validate(value2));
}

TEST_CASE(validate_date_string_format) {
	auto data_schema = dom_parser::from_string("{\"type\" : \"string\", \"format\" : \"date\" }").json_val;
	json_schema_validator data_validator(data_schema);
	auto value1 = dom_parser::from_string("\"2001-12-22\"").json_val;
	auto value2 = dom_parser::from_string("\"20-45-9665\"").json_val;
	TEST_ASSERT(data_validator.validate(value1));
	TEST_ASSERT(!data_validator.validate(value2));
}

TEST_CASE(validate_time_string_format) {
	auto time_schema = dom_parser::from_string("{\"type\" : \"string\", \"format\" : \"time\" }").json_val;
	json_schema_validator time_validator(time_schema);
	auto value1 = dom_parser::from_string("\"17:00:00Z\"").json_val;
	auto value2 = dom_parser::from_string("\"156:001:00V\"").json_val;
	TEST_ASSERT(time_validator.validate(value1));
	TEST_ASSERT(!time_validator.validate(value2));
}

TEST_CASE(validate_email_string_format) {
	auto schema = dom_parser::from_string("{\"type\" : \"string\", \"format\" : \"email\" }").json_val;
	json_schema_validator validator(schema);
	auto value1 = dom_parser::from_string("\"example@email.com\"").json_val;
	auto value2 = dom_parser::from_string("\"email@a@m.ru\"").json_val;
	TEST_ASSERT(validator.validate(value1));
	TEST_ASSERT(!validator.validate(value2));
}

TEST_CASE(validate_example_string_format) {
	auto schema = dom_parser::from_string("{\"type\" : \"string\", \"format\" : \"hostname\" }").json_val;
	json_schema_validator validator(schema);
	auto value1 = dom_parser::from_string("\"example.com\"").json_val;
	auto value2 = dom_parser::from_string("\"examplerucom\"").json_val;
	TEST_ASSERT(validator.validate(value1));
	TEST_ASSERT(!validator.validate(value2));
}

TEST_CASE(validate_ipv4_string_format) {
	auto schema = dom_parser::from_string("{\"type\" : \"string\", \"format\" : \"ipv4\" }").json_val;
	json_schema_validator validator(schema);
	auto value1 = dom_parser::from_string("\"192.168.1.1\"").json_val;
	auto value2 = dom_parser::from_string("\"12569.247.asd\"").json_val;
	TEST_ASSERT(validator.validate(value1));
	TEST_ASSERT(!validator.validate(value2));
}

TEST_CASE(validate_ipv6_string_format) {
	auto schema = dom_parser::from_string("{\"type\" : \"string\", \"format\" : \"ipv6\" }").json_val;
	json_schema_validator validator(schema);
	auto value1 = dom_parser::from_string("\"2001:db8::1\"").json_val;
	auto value2 = dom_parser::from_string("\"56987:sdsd::2\"").json_val;
	TEST_ASSERT(validator.validate(value1));
	TEST_ASSERT(!validator.validate(value2));
}

TEST_CASE(validate_uri_string_format) {
	auto schema = dom_parser::from_string("{\"type\" : \"string\", \"format\" : \"uri\" }").json_val;
	json_schema_validator validator(schema);
	auto value1 = dom_parser::from_string("\"https://example.com\"").json_val;
	auto value2 = dom_parser::from_string("\"htps:/q/example.com\"").json_val;
	TEST_ASSERT(validator.validate(value1));
	TEST_ASSERT(!validator.validate(value2));
}

TEST_CASE(validate_uuid_string_format) {
	auto schema = dom_parser::from_string("{\"type\" : \"string\", \"format\" : \"uuid\" }").json_val;
	json_schema_validator validator(schema);
	auto value1 = dom_parser::from_string("\"123e4567-e89B-12d3-a456-426614174000\"").json_val;
	auto value2 = dom_parser::from_string("\"123e4d56QQQQ7-e89-1dE2d3-a45E6-4266d1QWQWE417t4000\"").json_val;
	TEST_ASSERT(validator.validate(value1));
	TEST_ASSERT(!validator.validate(value2));
}

TEST_CASE(validate_array_min_max_size_test){
	auto schema = dom_parser::from_string("{\"type\" : \"array\", \"minItems\" : 3, \", \"maxItems\" : 5 }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("[\"ussr\", \"lenin\", 1917, 7, \"yra\"]").json_val;
	auto no_valid = dom_parser::from_string("[\"usa\", \"burger\"]").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_array_items_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"array\", \"items\" : { \"type\" : \"number\" } }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("[1, 1,  -98]").json_val;
	auto no_valid = dom_parser::from_string("[\"1\", 98, 45]").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_array_items_tuple_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"array\", \"items\" : [{ \"type\" : \"number\" }, { \"type\" : \"number\" }] }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("[1, 2]").json_val;
	auto no_valid = dom_parser::from_string("[\"1\", 45]").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_array_additionalItems_fasle_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"array\", \"items\" : [{ \"type\" : \"number\" }, { \"type\" : \"number\" }], \"additionalItems\" : false }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("[1, 2]").json_val;
	auto no_valid = dom_parser::from_string("[ -45, 5, 5]").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_array_additionalItems_true_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"array\", \"items\" : [{ \"type\" : \"number\" }, { \"type\" : \"number\" }], \"additionalItems\" : true }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("[1, 2]").json_val;
	auto also_valid = dom_parser::from_string("[ -45, 5, 5]").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(validator.validate(also_valid));
}

TEST_CASE(validate_array_additionalItems_object_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"array\", \"items\" : [{ \"type\" : \"number\" }, { \"type\" : \"number\" }], \"additionalItems\" : { \"type\" : \"string\" } }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("[1, 2, \"string\" ]").json_val;
	auto also_valid = dom_parser::from_string("[ -45, 5]").json_val;
	auto no_valid = dom_parser::from_string("[ -45, 5, 5]").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(validator.validate(also_valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_array_uniqueItems_fasle_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"array\", \"items\" : [{ \"type\" : \"number\" }, { \"type\" : \"number\" }], \"uniqueItems\" : false }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("[1, 2]").json_val;
	auto also_valid = dom_parser::from_string("[ -45, 5, 5]").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(validator.validate(also_valid));
}

TEST_CASE(validate_array_uniqueItems_true_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"array\", \"items\" : [{ \"type\" : \"number\" }, { \"type\" : \"number\" }], \"uniqueItems\" : true }").json_val;
	json_schema_validator validator(schema);
	auto no_valid = dom_parser::from_string("[1, 2]").json_val;
	auto also_no_valid = dom_parser::from_string("[ -45, 5, 5]").json_val;
	TEST_ASSERT(!validator.validate(no_valid));
	TEST_ASSERT(!validator.validate(also_no_valid));
}

TEST_CASE(validate_array_containts_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"array\", \"contains\" :  { \"type\" : \"string\" } }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("[1, 2, \"str\"]").json_val;
	auto no_valid = dom_parser::from_string("[ -45, 5, 5]").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_object_propertyNames_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"object\", \"propertyNames\" :  { \"minLength\" : 2, \"maxLength\" : 4 } }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("{ \"key\" : 5, \"key1\" : \"str\" }").json_val;
	auto no_valid = dom_parser::from_string("{ \"long key\" : 5, \"ddkey1\" : \"str\" }").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_object_minmaxProperties_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"object\", \"minProperties\" : 2, \"maxProperties\" : 4 }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("{ \"key\" : 5, \"key1\" : \"str\" }").json_val;
	auto no_valid = dom_parser::from_string("{ \"key\" : 5 }").json_val;
	auto also_no_valid = dom_parser::from_string("{ \"key\" : 5, \"key1\": 5,\"key2\": 5,\"key3\": 5,\"key4\": 5 }").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
	TEST_ASSERT(!validator.validate(also_no_valid));
}

TEST_CASE(validate_object_properties_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"object\", \"properties\": { \"age\" : {\"type\" : \"number\"} } }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("{ \"age\" : 5,  }").json_val;
	auto no_valid = dom_parser::from_string("{ \"age\" : \"5\"}").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_object_patternProperties_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"object\", \"patternProperties\": { \"^I_.*\" : {\"type\" : \"number\"} } }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("{ \"I_age\" : 5,  }").json_val;
	auto no_valid = dom_parser::from_string("{ \"I_age\" : \"5\"}").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_object_additionalProperties_false_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"object\", \"properties\": { \"age\" : {\"type\" : \"number\"} }, \"additionalProperties\": false }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("{ \"age\" : 5,  }").json_val;
	auto no_valid = dom_parser::from_string("{ \"age\" : 5, \"a\": 4}").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_object_additionalProperties_true_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"object\", \"properties\": { \"age\" : {\"type\" : \"number\"} }, \"additionalProperties\": true }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("{ \"age\" : 5,  }").json_val;
	auto also_valid = dom_parser::from_string("{ \"age\" : 5, \"a\": 4}").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(validator.validate(also_valid));
}

TEST_CASE(validate_object_additionalProperties_object_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"object\", \"properties\": { \"age\" : {\"type\" : \"number\"} }, \"additionalProperties\": { \"type\" : \"number\" } }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("{ \"age\" : 5,  \"a\": 4 }").json_val;
	auto no_valid = dom_parser::from_string("{ \"age\" : 5, \"a\": \"4\"}").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_object_required_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"object\", \"required\": [ \"age\", \"name\"] }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("{ \"age\" : 14, \"name\" : \"kirill\" }").json_val;
	auto no_valid = dom_parser::from_string("{ \"age\" : \"5\"}").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(validate_object_dependencies_test) {
	auto schema = dom_parser::from_string("{\"type\" : \"object\", \"dependencies\": { \"a\" : [\"b\", \"c\"] } }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string("{ \"a\" : { \"b\" : 1, \"c\" : \"str\" } }").json_val;
	auto no_valid = dom_parser::from_string("{ \"a\" : { \"c\" : \"str\" } }").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(allOf_test) {
	auto schema = dom_parser::from_string("{ \"allOf\" : [  { \"type\" : \"number\" } , { \"multipleOf\" : 2 } ] }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string(" 4 ").json_val;
	auto no_valid = dom_parser::from_string(" 3 ").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(anyOf_test) {
	auto schema = dom_parser::from_string("{ \"anyOf\" : [  { \"type\" : \"number\" } , { \"multipleOf\" : 2 } ] }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string(" 4 ").json_val;
	auto also_valid = dom_parser::from_string(" 3 ").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(validator.validate(also_valid));
}

TEST_CASE(oneOf_test) {
	auto schema = dom_parser::from_string("{ \"oneOf\" : [  { \"type\" : \"number\" } , { \"multipleOf\" : 2 } ] }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string(" 3 ").json_val;
	auto no_valid = dom_parser::from_string(" 4 ").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(not_test) {
	auto schema = dom_parser::from_string("{ \"not\" :  { \"type\" : \"number\" } }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string(" \"string\" ").json_val;
	auto no_valid = dom_parser::from_string(" 3 ").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}

TEST_CASE(if_test) {
	auto schema = dom_parser::from_string("{ \"if\" :  { \"type\" : \"number\" }, \"then\" : { \"multipleOf\" : 2 }  }").json_val;
	json_schema_validator validator(schema);
	auto valid = dom_parser::from_string(" 4 ").json_val;
	auto no_valid = dom_parser::from_string(" 3 ").json_val;
	TEST_ASSERT(validator.validate(valid));
	TEST_ASSERT(!validator.validate(no_valid));
}