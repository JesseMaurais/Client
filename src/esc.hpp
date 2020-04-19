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
	string::out enc(string::out out)
	{
		return (out << ... << Code);
	}

	template
	<
		int... Params
	>
	auto par(int param, Params... params)
	{
		if constexpr (0 < sizeof...(Params))
		{
			return out << param << ';' << par(params...);
		}
		else
		{
			return out << param;
		}
	}

	template
	<
		char Code, char Escape, int... Params
	>
	auto esc(Params... params)
	{
		using begin = enc<C0::ESC, Code>;
		using next = par<Params...>;
		using end = enc<Escape>;

		return out << begin << next(params) << end;
	}

	template
	<
		char Escape, int... Params
	>
	auto ctl(Params... params)
	{
		return [=](string::out out) -> string::out
		{
			return out << esc<G0::CSI, Escape>(params...);
		};
	}

	namespace cursor
	{
		inline auto up       = ctl<CSI::CUU, int>; // up rows
		inline auto down     = ctl<CSI::CUD, int>; // down rows
		inline auto back     = ctl<CSI::CUB, int>; // back columns
		inline auto next     = ctl<CSI::CNL, int>; // next line
		inline auto previous = ctl<CSI::CPL, int>; // previous line
		inline auto habs     = ctl<CSI::CHA, int>; // horizontal absolute
		inline auto htab     = ctl<CSI::CHT, int>; // horizontal tab
	}

	namespace page
	{
		inline auto up   = ctl<CSI::SU> // scroll up
		inline auto down = ctl<CSI::SD> // scroll down
		inline auto 
	}

	template 
	<
		int... Params
	>
	string::out set(string::out out);
	{
		return out << ctl<CSI::SGR>(Params...);
	}

	constexpr auto reset = set<SGR::reset>;
	constexpr auto intense = set<SGR::intense>;
	constexpr auto faint = set<SGR::faint>;
	constexpr auto italic = set<SGR::italic>;
	constexpr auto underline = set<SGR::underline>;
	constexpr auto blink_slow = set<SGR::blink_slow>;
	constexpr auto blink_fast = set<SGR::blink_fast>;
	constexpr auto reverse = set<SGR::reverse>;
	constexpr auto conceal = set<SGR::conceal>;
	constexpr auto strike = set<SGR::strike>;
	template <int N> constexpr auto font = set<SGR::first_font + N>;
	constexpr auto fraktur = set<SGR::fraktur>;
	constexpr auto underline2 = set<SGR::underline2>;
	constexpr auto intense_off = set<SGR::intense_off>;
	constexpr auto italic_off = set<SGR::italic_off>;
	constexpr auto underline_off = set<SGR::underline_off>;
	constexpr auto blink_off = set<SGR::blink_off>;
	constexpr auto inverse_off = set<SGR::inverse_off>;
	constexpr auto reveal = set<SGR::reveal>;
	constexpr auto strike_off = set<SGR::strike_off>;
	constexpr auto fg_black = set<SGR::fg_black>;
	constexpr auto fg_red = set<SGR::fg_red>;
	constexpr auto fg_green = set<SGR::fg_green>;
	constexpr auto fg_yellow = set<SGR::fg_yellow>;
	constexpr auto fg_blue = set<SGR::fg_blue>;
	constexpr auto fg_magenta = set<SGR::fg_magenta>;
	constexpr auto fg_cyan = set<SGR::fg_cyan>;
	constexpr auto fg_white = set<SGR::fg_white>;
	template <int R, int G, int B> constexpr auto fg = set<SGR::fg, 2, R, G, B>;
	constexpr auto fg_off = set<SGR::fg_off>;
	constexpr auto bg_black = set<SGR::bg_black>;
	constexpr auto bg_red = set<SGR::bg_red>;
	constexpr auto bg_green = set<SGR::bg_green>;
	constexpr auto bg_yellow = set<SGR::bg_yellow>;
	constexpr auto bg_blue = set<SGR::bg_blue>;
	constexpr auto bg_magenta = set<SGR::bg_magenta>;
	constexpr auto bg_cyan = set<SGR::bg_cyan>;
	constexpr auto bg_white = set<SGR::bg_white>;
	template <int R, int G, int B> constexpr auto bg = set<SGR::bg, 2, R, G, B>;
	constexpr auto bg_off = set<SGR::bg_off>;
	constexpr auto frame = set<SGR::frame>;
	constexpr auto encircle = set<SGR::encircle>;
	constexpr auto overline = set<SGR::overline>;
	constexpr auto frame_off = set<SGR::frame_off>;
	constexpr auto overline_off = set<SGR::overline_off>;

}

#endif // file

