#ifndef sh_hpp
#define sh_hpp

#include <ostream>
#include "esc.hpp"

namespace sh
{
	using namespace ascii;

	template <char... Code> inline std::ostream& encode(std::ostream& out)
	{
		return (out << ... << Code);
	}

	template <int Code, int... Params> inline std::ostream& params(std::ostream& out)
	{
		if constexpr (0 < sizeof...(Params))
		{
			return out << Code << ';' << params<Params...>;
		}
		else
		{
			return out << Code;
		}
	}

	template <SGR... Code> inline std::ostream& graphic(std::ostream& out)
	{
		return out << encode<C0::ESC, G0::CSI> << params<Code...> << encode<CSI::SGR>;
	}

	constexpr auto reset = graphic<SGR::reset>;
	constexpr auto intense = graphic<SGR::intense>;
	constexpr auto faint = graphic<SGR::faint>;
	constexpr auto intense_off = graphic<SGR::intense_off>;
	constexpr auto fg_back = graphic<SGR::fg_black>;
	constexpr auto fg_red = graphic<SGR::fg_red>;
	constexpr auto fg_green = graphic<SGR::fg_green>;
	constexpr auto fg_yellow = graphic<SGR::fg_yellow>;
	constexpr auto fg_blue = graphic<SGR::fg_blue>;
	constexpr auto fg_magenta = graphic<SGR::fg_magenta>;
	constexpr auto fg_cyan = graphic<SGR::fg_cyan>;
	constexpr auto fg_white = graphic<SGR::fg_white>;
	constexpr auto fg_off = graphic<SGR::fg_off>;
	template <int R, int G, int B> inline std::ostream& fg(std::ostream& out)
	{
		return out << encode<C0::ESC, G0::CSI> << params<SGR::fg, 2, R, G, B> << encode<CSI::SGR>;
	}
}

#endif // file

