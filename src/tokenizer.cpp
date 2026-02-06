#include "tokenizer.h"

using namespace json::io;
using namespace json::encodings;

json::io::tokenizer::tokenizer(i_input_processor_ptr_ref input_proc,
							   i_decoder_ptr_ref decoder,
							   size_t & line, 
							   size_t & col)
:_input_proc(input_proc), _decoder(decoder), _line(line), _col(col){}

json::io::tokenizer::tokenizer(const tokenizer & tok) 
	: _input_proc(tok._input_proc), _decoder(tok._decoder), _line(tok._line), _col(tok._col) {}

token & json::io::tokenizer::next(){
	cur_token = _input_proc->next_token(_decoder);
	return cur_token;
}

token & json::io::tokenizer::last(){
	return cur_token;
}
