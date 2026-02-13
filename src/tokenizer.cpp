#include "tokenizer.h"

using namespace json::io;
using namespace json::encodings;

json::io::tokenizer::tokenizer(i_input_processor_ptr_ref input_proc,
							   i_decoder_ptr_ref decoder,
							   std::vector<std::unique_ptr<base_error>> & errors)
:_input_proc(input_proc), _decoder(decoder), _errors(errors) {}

json::io::tokenizer::tokenizer(const tokenizer & tok) 
	: _input_proc(tok._input_proc), _decoder(tok._decoder), _errors(tok._errors) {}

token & json::io::tokenizer::next(){
	bool _success = false;
	while (!_success) {
		try {
			cur_token = _input_proc->next_token(_decoder);
			_success = true;
		}
		catch (json::base_error & err) {
			_errors.push_back(std::make_unique<json::base_error>(err));
		}
	}
	return cur_token;
}

token & json::io::tokenizer::last(){
	return cur_token;
}
