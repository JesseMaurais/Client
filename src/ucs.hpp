#ifndef ucs_hpp
#define ucs_hpp

#include "str.hpp"

namespace fmt::unicode
{
	using point = char32_t;

	template <point Min, point Max> struct range : range<point>
	{
		using base = range<point>;
		using base::base;
		range() : base(Min, Max) { }
	};

	using codespace = range<0x000000, 0x10FFFF>;

	// special chars

	constexpr point BOM = 0x00FEFF; // byte order mark

	// allocations

	using BMP  = range<0x000000, 0x00FFFF>; // basic multilingual plane
	using SMP  = range<0x010000, 0x01FFFF>; // supplementary multilingual plane
	using SIP  = range<0x020000, 0x02FFFF>; // supplementary ideographic plane
	using TIP  = range<0x030000, 0x03FFFF>; // tertiary ideographic plane
	using UNP  = range<0x040000, 0x0DFFFF>; // unassigned planes
	using SSP  = range<0x0E0000, 0x0EFFFF>; // supplementary special purpose plane
	using SPUA = range<0x0F0000, 0x10FFFF>; // supplementary private use area planes

	// BMP plane

	using ACA  = range<0x000000, 0x0000FF>; // ASCII & Latin-1 compatibility area
	using GSA  = range<0x000100, 0x00058F>; // general scripts area
	using RTL  = range<0x000590, 0x0008FF>; // general scripts area (RTL)
	using GSA2 = range<0x000900, 0x001FFF>; // general scripts area
	using PSA  = range<0x002000, 0x002BFF>; // punctuation and symbols area
	using GSA3 = range<0x002C00, 0x002DFF>; // general scripts area
	using SPA  = range<0x002E00, 0x002E7F>; // supplemental punctuation area
	using CJK  = range<0x002E80, 0x0033FF>; // CJK miscellaneous area
	using CJKV = range<0x003400, 0x009FFF>; // CJKV unified ideographs area
	using GSA4 = range<0x00A000, 0x00ABFF>; // general scripts area (Asia &Africa)
	using HSA  = range<0x00AC00, 0x00D7FF>; // Hangul syllables area
	using SC   = range<0x00D800, 0x00DFFF>; // surrogate codes
	using PUA  = range<0x00E000, 0x00F8FF>; // private use area
	using CSA  = range<0x00F900, 0x00FFFF>; // compatibility and specials area

	// SMP plane

	using GSA5 = range<0x010000, 0x0107FF>; // general scripts area
	using RTL2 = range<0x010800, 0x010FFF>; // general scripts area (RTL)
	using GSA6 = range<0x011000, 0x011FFF>; // general scripts area
	using CHA  = range<0x012000, 0x015FFF>; // cuneiform & hieroglyphic area
	using GSA7 = range<0x016000, 0x016FFF>; // general scripts area
	using ISA  = range<0x017000, 0x01BBFF>; // ideographic scripts area
	using GSA8 = range<0x01BC00, 0x01CFFF>; // general scripts area
	using SA   = range<0x01D000, 0x01E7FF>; // symbols area
	using RTL3 = range<0x01E800, 0x01EFFF>; // general scripts area
	using SA2  = range<0x01F000, 0x01FFFF>; // symbols area

	// SPUA planes

	using PUAA = range<0x0F0000, 0x0FFFFF>; // private use area A
	using PUAB = range<0x100000, 0x10FFFF>; // private use area B
};

#endif // file
