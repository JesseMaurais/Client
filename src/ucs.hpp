#ifndef ucs_hpp
#define ucs_hpp "Universal Character Set"

#include "fmt.hpp"
#include "it.hpp"

namespace fmt::uni
{
	template 
	<
		wint_t Min, wint_t Max
	> 
	using plane = fwd::closure<Min, Max>;

	template
	<
		class... Planes
	>
	struct compose = fwd::compose<Planes...>;

	using space = plane<0x000000, 0x10FFFF>;

	// special chars

	constexpr wint_t BOM = 0x00FEFF; // byte order mark

	// Plane allocations

	using BMP   = plane<0x000000, 0x00FFFF>; // basic multilingual plane
	using SMP   = plane<0x010000, 0x01FFFF>; // supplementary multilingual plane
	using SIP   = plane<0x020000, 0x02FFFF>; // supplementary ideographic plane
	using TIP   = plane<0x030000, 0x03FFFF>; // tertiary ideographic plane
	using UNP   = plane<0x040000, 0x0DFFFF>; // unassigned planes
	using SSP   = plane<0x0E0000, 0x0EFFFF>; // supplementary special purpose plane
	using SPUA  = plane<0x0F0000, 0x10FFFF>; // supplementary private use area planes

	// BMP plane

	using ACA   = plane<0x000000, 0x0000FF>; // ASCII & Latin-1 compatibility area
	using GSA_0 = plane<0x000100, 0x00058F>; // general scripts area
	using RTL_0 = plane<0x000590, 0x0008FF>; // general scripts area (RTL)
	using GSA_1 = plane<0x000900, 0x001FFF>; // general scripts area
	using PSA   = plane<0x002000, 0x002BFF>; // punctuation and symbols area
	using GSA_2 = plane<0x002C00, 0x002DFF>; // general scripts area
	using SPA   = plane<0x002E00, 0x002E7F>; // supplemental punctuation area
	using CJK   = plane<0x002E80, 0x0033FF>; // CJK miscellaneous area
	using CJKV  = plane<0x003400, 0x009FFF>; // CJKV unified ideographs area
	using GSA_3 = plane<0x00A000, 0x00ABFF>; // general scripts area (Asia &Africa)
	using HSA   = plane<0x00AC00, 0x00D7FF>; // Hangul syllables area
	using SC    = plane<0x00D800, 0x00DFFF>; // surrogate codes
	using PUA   = plane<0x00E000, 0x00F8FF>; // private use area
	using CSA   = plane<0x00F900, 0x00FFFF>; // compatibility and specials area

	// SMP plane

	using GSA_5 = plane<0x010000, 0x0107FF>; // general scripts area
	using RTL_1 = plane<0x010800, 0x010FFF>; // general scripts area (RTL)
	using GSA_6 = plane<0x011000, 0x011FFF>; // general scripts area
	using CHA   = plane<0x012000, 0x015FFF>; // cuneiform & hieroglyphic area
	using GSA_7 = plane<0x016000, 0x016FFF>; // general scripts area
	using ISA   = plane<0x017000, 0x01BBFF>; // ideographic scripts area
	using GSA_8 = plane<0x01BC00, 0x01CFFF>; // general scripts area
	using SA_0  = plane<0x01D000, 0x01E7FF>; // symbols area
	using RTL_2 = plane<0x01E800, 0x01EFFF>; // general scripts area
	using SA_1  = plane<0x01F000, 0x01FFFF>; // symbols area

	// SPUA planes

	using PUA_A = plane<0x0F0000, 0x0FFFFF>; // private use area A
	using PUA_B = plane<0x100000, 0x10FFFF>; // private use area B

	// Compound

	using GSA  = compose<GSA_0, GSA_1, GSA_2, GSA_3, GSA_4, GSA_5, GSA_6, GSA_7, GSA_8>;
	using RTL  = compose<RTL_0, RTL_1, RTL_2>;
	using SA   = compose<SA_0, SA_1>;
	using PUA  = compose<PUA_A, PUA_B>;
};

#endif // file
