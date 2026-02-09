#ifndef __DRONJSON_PARSER_IMPL_
#define __DRONJSON_PARSER_IMPL_

#include "error.h"
#include "tokenizer.h"

namespace json {

	namespace impl {

		class parse_error : public base_error {

		};

		class dom_parser_impl {
		public:

			// конструктор
			dom_parser_impl(std::unique_ptr<json::io::tokenizer> );
			
		//	parse();

		private:
			std::unique_ptr<json::io::tokenizer> _tokenizer;
		};

	} // impl

} // json

#endif // __DRONJSON_PARSER_IMPL_
