#include "core\tokenizer.h"

using namespace json;
using namespace json::core::io;
using namespace json::core::io::encodings;

tokenizer::tokenizer(i_input_processor_ptr_ref input_proc,
							   i_decoder_ptr_ref decoder,
							   std::vector<std::unique_ptr<error>> & errors)
:_input_proc(input_proc), _decoder(decoder), _errors(errors) {}

tokenizer::tokenizer(const tokenizer & tok) 
	: _input_proc(tok._input_proc), _decoder(tok._decoder), _errors(tok._errors) {}

token & tokenizer::next(){
	bool _success = false;
	while (!_success) {
		try {
			cur_token = _input_proc->next_token(_decoder);
			_success = true;
		}
		catch (error & err) {
			_errors.push_back(std::make_unique<json::error>(err));
		}
	}
	return cur_token;
}

token & tokenizer::last(){
	return cur_token;
}
