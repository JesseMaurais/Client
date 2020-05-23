#ifndef esc_hpp
#define esc_hpp "Escape Control Codes"

#include <iomanip>
#include "fmt.hpp"
#include "acs.hpp"

namespace fmt
{
	template 
	<
		char... Code
	>
	string::out::ref enc(string::out::ref out)
	{
		return (out << ... << Code);
	}

	template
	<
		int Param, int... Params
	>
	string::out::ref par(string::out::ref out)
	{
		out << Param;
		if constexpr (0 < sizeof...(Params))
		{
			return ((out << ';' << Params), ...);
		}
		return out;
	}

	template
	<
		char Code, char Escape, int... Params
	>
	string::out::ref esc(string::out::ref out)
	{
		auto begin = enc<uni::C0::ESC, Code>;
		auto next = par<Params...>;
		auto end = enc<Escape>;

		return out << begin << next << end;
	}

	template
	<
		char Escape, int... Params
	>
	string::out::ref ctl(string::out::ref out)
	{
		return out << esc<uni::G0::CSI, Escape, Params...>;
	}

	template 
	<
		int... Params
	>
	string::out::ref set(string::out::ref out)
	{
		return out << ctl<uni::CSI::SGR, Params...>;
	}

	constexpr auto reset = set<uni::SGR::reset>;
	constexpr auto intense = set<uni::SGR::intense>;
	constexpr auto faint = set<uni::SGR::faint>;
	constexpr auto italic = set<uni::SGR::italic>;
	constexpr auto underline = set<uni::SGR::underline>;
	constexpr auto blink_slow = set<uni::SGR::blink_slow>;
	constexpr auto blink_fast = set<uni::SGR::blink_fast>;
	constexpr auto reverse = set<uni::SGR::reverse>;
	constexpr auto conceal = set<uni::SGR::conceal>;
	constexpr auto strike = set<uni::SGR::strike>;
	template <int N> constexpr auto font = set<uni::SGR::first_font + N>;
	constexpr auto fraktur = set<uni::SGR::fraktur>;
	constexpr auto underline2 = set<uni::SGR::underline2>;
	constexpr auto intense_off = set<uni::SGR::intense_off>;
	constexpr auto italic_off = set<uni::SGR::italic_off>;
	constexpr auto underline_off = set<uni::SGR::underline_off>;
	constexpr auto blink_off = set<uni::SGR::blink_off>;
	constexpr auto inverse_off = set<uni::SGR::inverse_off>;
	constexpr auto reveal = set<uni::SGR::reveal>;
	constexpr auto strike_off = set<uni::SGR::strike_off>;
	constexpr auto fg_black = set<uni::SGR::fg_black>;
	constexpr auto fg_red = set<uni::SGR::fg_red>;
	constexpr auto fg_green = set<uni::SGR::fg_green>;
	constexpr auto fg_yellow = set<uni::SGR::fg_yellow>;
	constexpr auto fg_blue = set<uni::SGR::fg_blue>;
	constexpr auto fg_magenta = set<uni::SGR::fg_magenta>;
	constexpr auto fg_cyan = set<uni::SGR::fg_cyan>;
	constexpr auto fg_white = set<uni::SGR::fg_white>;
	template <int R, int G, int B> constexpr auto fg = set<uni::SGR::fg, 2, R, G, B>;
	constexpr auto fg_off = set<uni::SGR::fg_off>;
	constexpr auto bg_black = set<uni::SGR::bg_black>;
	constexpr auto bg_red = set<uni::SGR::bg_red>;
	constexpr auto bg_green = set<uni::SGR::bg_green>;
	constexpr auto bg_yellow = set<uni::SGR::bg_yellow>;
	constexpr auto bg_blue = set<uni::SGR::bg_blue>;
	constexpr auto bg_magenta = set<uni::SGR::bg_magenta>;
	constexpr auto bg_cyan = set<uni::SGR::bg_cyan>;
	constexpr auto bg_white = set<uni::SGR::bg_white>;
	template <int R, int G, int B> constexpr auto bg = set<uni::SGR::bg, 2, R, G, B>;
	constexpr auto bg_off = set<uni::SGR::bg_off>;
	constexpr auto frame = set<uni::SGR::frame>;
	constexpr auto encircle = set<uni::SGR::encircle>;
	constexpr auto overline = set<uni::SGR::overline>;
	constexpr auto frame_off = set<uni::SGR::frame_off>;
	constexpr auto overline_off = set<uni::SGR::overline_off>;
}

#endif // file

