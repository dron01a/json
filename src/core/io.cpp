#include "core/io.h"

using namespace json;
using namespace json::core;
using namespace json::core::io;
using namespace json::core::io::io_base;
using namespace json::core::io::encodings;

i_decoder_ptr io::make_decoder(encoding enc, input & input) {
	switch (enc) {
	case json::encoding::ascii:
		return std::make_unique<ascii_decoder>(input);
	case json::encoding::utf8:
		return std::make_unique<utf8_decoder>(input);
	}

}

i_encoder_ptr io::make_encoder(encoding enc, i_output_ptr_ref output) {
	switch (enc) {
	case json::encoding::ascii:
		return std::make_unique<ascii_encoder>(output);
	case json::encoding::utf8:
		return std::make_unique<utf8_encoder>(output);
	};
}

i_input_processor_ptr io::make_input_processor(json_sinax sinax) {
	switch (sinax) {
	case json::json_sinax::JSON:
		return std::make_unique<json_input_processor>();
	case json::json_sinax::JSON5:
		return std::make_unique<json5_input_processor>();
	}

}

i_output_processor_ptr io::make_output_processor(output_format format, i_encoder_ptr_ref dest, bool format_flag) {
	switch (format) {
	case json::output_format::JSON:
		return std::make_unique<json_output_processor>(dest, format_flag);
	case json::output_format::XML:
		return std::make_unique<xml_output_processor>(dest, format_flag);
	}
}

