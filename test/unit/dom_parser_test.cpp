#include <iostream>
#include <cassert>

#include "input_processor.h"

#include "../tools/tests_framework.h"
#include "../tools/mock_tools.h"

TEST_CASE() {
	class test_proc : public base_input_processor {
	public:
		token next_token(encodings::i_decoder_ptr_ref _decoder) { return token(); };
		using base_input_processor::parse_string;
	};
	
}