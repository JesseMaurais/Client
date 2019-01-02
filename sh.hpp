#ifndef sh_hpp
#define sh_hpp

#include <ostream>
#include "esc.hpp"

namespace sh
{
	using namespace ascii;

	template <SGR Code> inline std::ostream& encode(std::ostream& out)
	{
		return out << (char) C0::ESC << (char) G0::CSI << Code << (char) CSI::SGR;
	}

	constexpr auto reset = encode<SGR::reset>;
	constexpr auto fg_red = encode<SGR::fg_red>;
	constexpr auto fg_green = encode<SGR::fg_green>;
	constexpr auto fg_yellow = encode<SGR::fg_yellow>;
	constexpr auto fg_off = encode<SGR::fg_off>;
}

#endif // file

