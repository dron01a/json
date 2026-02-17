//#include "encoding.h"
//#include "input_processor.h"
//#include "tokenizer.h"

#include "io_base.h"
#include "encoding.h"
#include "input_processor.h"
#include "tokenizer.h"

#include "parser.h"
#include "json_value.h"
#include "json.h"

#include <iostream>

int main() {
//	setlocale(LC_ALL, "Ru-ru");
	//size_t i = 0, j = 0;

	//json::json_value val(1.0);

	//std::cout << val.as_num() << std::endl;

	//val.as_string("asas");

	//std::cout << val.as_string() << std::endl;

	//val.type(json::value_type::_bool);

	//std::cout << val.as_bool() << std::endl;

	////json::encodings::ascii::encoder encod;
	////json::encodings::utf8::decoder decod;
	////json::io_base::string_input * str_in = new json::io_base::string_input(" \"json\" : { \"a\" : 2, \"12\" : 2, }");
	////std::unique_ptr<json::io_base::i_input> str_in_up(new json::io_base::string_input(" \"json\" : { \"a\" : 2, \"12\" : 2, }"));
	////json::io::simple_input_processor sip(str_in, &decod, i, j);
	/////*str_in->next_char();
	////bool result = sip.literal_processing("true");
	////std::cout << result << std::endl;*/
	////std::unique_ptr<json::io::i_input_processor> sip_up(new json::io::simple_input_processor(str_in, &decod, i, j));
	////json::io::tokenizer tokns(
	////	new json::io::simple_input_processor(str_in, &decod, i, j),
	////	str_in,
	////	i, 
	////	j);
	////json::io::token t = tokns.next();1
	////while (t.type() != json::io::token_type::_end) {
	////	std::cout << (int)t.type() << std::endl;
	////	t = tokns.next();
	////}
	////uint8_t a = -1;
	////json::io_base::string_input * str_in = new json::io_base::string_input("655sdfsrtaАsdf");
	////std::unique_ptr<json::io_base::i_input> a = std::make_unique<json::io_base::string_input>(json::io_base::string_input(" \"json\" : { \"a\" : trasue, \"12\" : 2, }"));
	//////json::encodings::ascii_decoder dec(std::move(a));
	////std::unique_ptr<json::encodings::i_decoder> up_dec = std::make_unique<json::encodings::ascii_decoder>(std::move(a));
	/////*for (size_t i = 0; i < 10; i++) {
	////	std::cout << (char)str_in->next_char() << std::endl;
	////}*/
	//////std::cout << "655sdfsrtaАsdf" << std::endl;
	///*std::string s = "";
	//while(!up_dec->eof()) {
	//	s += up_dec->next_char();
	//}
	//std::cout << s << std::endl;*/


	///*json::io::json_input_processor jip(i, j);
	//json::io::token t(json::io::token_type::_none); 
	//std::unique_ptr<json::io::i_input_processor> up_jip = 
	//	std::make_unique<json::io::json_input_processor>(json::io::json_input_processor(i, j));
	//json::io::tokenizer tknz(up_jip, up_dec, i,j);
	//std::vector<json::io::token> vt;
	//try {
	//	while (t.type() != json::io::token_type::_end) {
	//		t = tknz.next();
	//		vt.push_back(t);
	//	}
	//}
	//catch (json::base_error & e) {
	//	std::cout << e.what() << std::endl;
	//}*/

	json::parse_config conf = json::standart();
	conf.encoding() = json::parse_config::encoding_mode::utf8;
	conf.error_halding() = json::parse_config::error_mode::collect;
	conf.sinax() = json::parse_config::sinax_mode::JSON5;

	json::parse_result res = json::dom_parser::from_file("test_json5.json", conf);
	
	for (size_t i = 0; i < res.errors.size(); ++i) {
		std::cout << res.errors[i]->what() << std::endl;
	}

	json::json_value * val = res.json_val.find("nan_obj");

	//double d_res = val->as_num();
	if (val->is_bool()) {
		std::cout << "bool" << std::endl;
	}
	if (val->is_string()) {
		std::cout << "string " << val->as_string() << std::endl;
	}
	if (val->is_object()) {
		std::cout << "object" << std::endl;
	}
	if (val->is_array()) {
		std::cout << "array" << std::endl;
	}
	if (val->is_number()) {
		std::cout << "num : " << val->as_num() << std::endl;
	}
	//std::cout << val->as_string() << std::endl;

	return 0;
}