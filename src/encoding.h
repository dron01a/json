#ifndef __DRON_JSON_ENCODING__
#define __DRON_JSON_ENCODING__

#include "io_base.h"

namespace json {

	namespace encodings {

	/*	class encoding_error : base_error {
		public:

			enum error_code {
				_invalid_string,
				_invalid_escape,
			};

		};*/

		// интерфейс декодера
		class i_decoder {
		public:
			virtual ~i_decoder() = default;
			virtual std::string decode(json::io_base::i_input * _src) = 0;
		};

		// интерфейс энкодера
		class i_encoder {
		public:
			virtual ~i_encoder() = default;
			virtual void encode(json::io_base::i_output * _dest, const std::string & _string) = 0;
		};

		namespace utf8 {

			// декодер utf8
			class decoder : public i_decoder {
			public:
				explicit decoder() {};
				std::string decode(json::io_base::i_input * _src);
			private:
				// функция для чтения юникода 
				std::string read_unicode(json::io_base::i_input * _src);
				// добавляет utf8 сивол к строке
				void append_utf8_char(std::string & _string, uint32_t code);
			};

			// энкодер utf8
			class encoder : public i_encoder {
			public:
				explicit encoder() {};
				void encode(json::io_base::i_output * _dest, const std::string & _string);
			private:
				inline void append_hex_4(json::io_base::i_output * _dest, uint16_t value);
				inline int get_utf8_len(unsigned char fb);
				inline uint32_t parse_utf8(const std::string & str, size_t & pos, int len);
			};

		};

		namespace ascii {

			// декодер ascii
			class decoder : public i_decoder {
			public:
				explicit decoder() {};
				std::string decode(json::io_base::i_input * _src);
			};

			// энкодер ascii
			class encoder : public i_encoder {
			public:
				explicit encoder() {};
				void encode(json::io_base::i_output * _dest, const std::string & _string);
			};

		};

	}

};

#endif // !__DRON_JSON_ENCODING__
