//#include "encoding.h"
//#include "input_processor.h"
//#include "tokenizer.h"

#include "io_base.h"
#include "encoding.h"
#include "input_processor.h"
#include "tokenizer.h"

#include <iostream>

int main() {
	setlocale(LC_ALL, "Ru-ru");
	size_t i = 0, j = 0;
	//json::encodings::ascii::encoder encod;
	//json::encodings::utf8::decoder decod;
	//json::io_base::string_input * str_in = new json::io_base::string_input(" \"json\" : { \"a\" : 2, \"12\" : 2, }");
	//std::unique_ptr<json::io_base::i_input> str_in_up(new json::io_base::string_input(" \"json\" : { \"a\" : 2, \"12\" : 2, }"));
	//json::io::simple_input_processor sip(str_in, &decod, i, j);
	///*str_in->next_char();
	//bool result = sip.literal_processing("true");
	//std::cout << result << std::endl;*/
	//std::unique_ptr<json::io::i_input_processor> sip_up(new json::io::simple_input_processor(str_in, &decod, i, j));
	//json::io::tokenizer tokns(
	//	new json::io::simple_input_processor(str_in, &decod, i, j),
	//	str_in,
	//	i, 
	//	j);
	//json::io::token t = tokns.next();1
	//while (t.type() != json::io::token_type::_end) {
	//	std::cout << (int)t.type() << std::endl;
	//	t = tokns.next();
	//}
	//uint8_t a = -1;
	json::io_base::string_input * str_in = new json::io_base::string_input("655sdfsrtaАsdf");
	std::unique_ptr<json::io_base::i_input> a = std::make_unique<json::io_base::string_input>(json::io_base::string_input(" \"json\" : { \"a\" : trasue, \"12\" : 2, }"));
	//json::encodings::ascii_decoder dec(std::move(a));
	std::unique_ptr<json::encodings::i_decoder> up_dec = std::make_unique<json::encodings::ascii_decoder>(std::move(a));
	/*for (size_t i = 0; i < 10; i++) {
		std::cout << (char)str_in->next_char() << std::endl;
	}*/
	//std::cout << "655sdfsrtaАsdf" << std::endl;
	/*std::string s = "";
	while(!up_dec->eof()) {
		s += up_dec->next_char();
	}
	std::cout << s << std::endl;*/
	
	json::io::json_input_processor jip(i, j);
	json::io::token t(json::io::token_type::_none); 
	std::unique_ptr<json::io::i_input_processor> up_jip = 
		std::make_unique<json::io::json_input_processor>(json::io::json_input_processor(i, j));
	json::io::tokenizer tknz(up_jip, up_dec, i,j);
	std::vector<json::io::token> vt;
	try {
		while (t.type() != json::io::token_type::_end) {
			t = tknz.next();
			vt.push_back(t);
		}
	}
	catch (json::base_error & e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}