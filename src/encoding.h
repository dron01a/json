#ifndef __DRON_JSON_ENCODING__
#define __DRON_JSON_ENCODING__

#include "io_base.h"

namespace json {

	namespace encodings {

		class encoding_error : base_error {
		public:

			enum error_code {  
				_invalid_string,
				_invalid_escape,
			};

		};

		// интерфейс декодера
		class i_decoder {
		public :
			virtual ~i_decoder() = default;
			virtual std::string decode(json::io_base::i_input * _src) = 0;
		};

		// интерфейс энкодера
		class i_encoder {
			virtual ~i_encoder() = default;
			virtual std::string encode(const std::string & _src) = 0;
		};

		namespace utf8 {

			// декодер utf8
			class decoder {
			public:
				explicit decoder() {};
				std::string decode(json::io_base::i_input * _src);
			};

			// энкодер utf8
			class encoder {
			public:
				explicit encoder() {};
				std::string encode(const std::string & _src);
			};

		};

		namespace ascii {

			// декодер ascii
			class decoder {
			public:
				explicit decoder() {};
				std::string decode(json::io_base::i_input * _src);
			};

			// энкодер ascii
			class encoder {
			public:
				explicit encoder() {};
				std::string encode(const std::string & _src);
			};

		};
		
	}

};

#endif // !__DRON_JSON_ENCODING__
