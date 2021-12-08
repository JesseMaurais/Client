#ifndef char_hpp
#define char_hpp "Universal Character Set"

#include "fmt.hpp"
#include "it.hpp"

namespace fmt
{
	using codepoint = int;

	template <codepoint Min, codepoint Max> using plane = fwd::interval<Min, Max>;

	using codespace = plane<0x000000, 0x10FFFF>;

	constexpr codepoint endpoint = 0x110000;

	// Special chars

	constexpr codepoint BOM = 0x00FEFF; // byte order mark

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
	using PUA_0 = plane<0x00E000, 0x00F8FF>; // private use area
	using CSA   = plane<0x00F900, 0x00FFFF>; // compatibility and specials area

	// SMP plane

	using GSA_4 = plane<0x010000, 0x0107FF>; // general scripts area
	using RTL_1 = plane<0x010800, 0x010FFF>; // general scripts area (RTL)
	using GSA_5 = plane<0x011000, 0x011FFF>; // general scripts area
	using CHA   = plane<0x012000, 0x015FFF>; // cuneiform & hieroglyphic area
	using GSA_6 = plane<0x016000, 0x016FFF>; // general scripts area
	using ISA   = plane<0x017000, 0x01BBFF>; // ideographic scripts area
	using GSA_7 = plane<0x01BC00, 0x01CFFF>; // general scripts area
	using SA_0  = plane<0x01D000, 0x01E7FF>; // symbols area
	using RTL_2 = plane<0x01E800, 0x01EFFF>; // general scripts area
	using SA_1  = plane<0x01F000, 0x01FFFF>; // symbols area

	// SPUA planes

	using PUA_A = plane<0x0F0000, 0x0FFFFF>; // private use area A
	using PUA_B = plane<0x100000, 0x10FFFF>; // private use area B

	// Compound planes

	using GSA  = fwd::intervals<GSA_0, GSA_1, GSA_2, GSA_3, GSA_4, GSA_5, GSA_6, GSA_7>;
	using RTL  = fwd::intervals<RTL_0, RTL_1, RTL_2>;
	using SA   = fwd::intervals<SA_0, SA_1>;
	using PUA  = fwd::intervals<PUA_0, PUA_A, PUA_B>;

	//
	// Character Codes
	//

	struct C0 : plane<'\x00', '\x1F'>
	{
		enum code : char
		{
			NUL = '\x00', // terminal character
			SOH = '\x01', // start of heading
			STX = '\x02', // start of text
			ETX = '\x03', // end of text
			EOT = '\x04', // end of transmission
			ENQ = '\x05', // enquiry
			ACK = '\x06', // acknowledge
			BEL = '\x07', // bell
			BS  = '\x08', // backspace
			HT  = '\x09', // horizontal tab
			LF  = '\x0A', // line feed/new line
			VT  = '\x0B', // vertical tab
			FF  = '\x0C', // form feed/new page
			CR  = '\x0D', // carriage return
			SI  = '\x0E', // shift in
			SO  = '\x0F', // shift out
			DLE = '\x10', // data link escape
			DC1 = '\x11', // device control 1
			DC2 = '\x12', // device control 2
			DC3 = '\x13', // device control 3
			DC4 = '\x14', // device control 4
			NAK = '\x15', // negative acknowledge
			SYN = '\x16', // synchronous idle
			ETB = '\x17', // end transmission block
			CAN = '\x18', // cancel
			EM  = '\x19', // end of medium
			SUB = '\x1A', // substitute
			ESC = '\x1B', // escape
			FS  = '\x1C', // file separator
			GS  = '\x1D', // group separator
			RS  = '\x1E', // record separator
			US  = '\x1F', // unit separator
			SP  = '\x20', // space
			DEL = '\x7F', // delete
		};
	};

	struct G0 : plane<'\x21', '\x7E'>
	{
		enum code : char
		{
			PAD = '\x40', // @ padding char
			HOP = '\x41', // A high octet preset
			BPH = '\x42', // B break permitted here
			NBH = '\x43', // C no break here
			NEL = '\x45', // E next line
			SSA = '\x46', // F start of selected area
			ESA = '\x47', // G end of selected area
			HTS = '\x48', // H horizontal tab set
			HTJ = '\x49', // I horizontal tab justified
			VTS = '\x4A', // J vertical tab set
			PLD = '\x4B', // K partial line down
			PLU = '\x4C', // L partial line up
			RI  = '\x4D', // M reverse line feed
			SS2 = '\x4E', // N single shift two
			SS3 = '\x4F', // O single shift three
			DCS = '\x50', // P device control string
			PU1 = '\x51', // Q private use one
			PU2 = '\x52', // R private use two
			STS = '\x53', // S set transmit state
			CCH = '\x54', // T cancel char
			MW  = '\x55', // U message waiting
			SPA = '\x56', // V start of guarded area
			EPA = '\x57', // W end of guarded area
			SOS = '\x58', // X start of string
			SCI = '\x5A', // Z single character introducer
			CSI = '\x5B', // [ control sequence introducer
			ST  = '\x5C', // \ string terminator
			OSC = '\x5D', // ] operating system command
			PM  = '\x5E', // ^ private message
			APC = '\x5F', // _ application program command
		};
	};

	struct C1 : plane<'\x80', '\x9F'>
	{
		enum code : char
		{
			PAD = '\x80', // @ padding char
			HOP = '\x81', // A high octet preset
			BPH = '\x82', // B break permitted here
			NBH = '\x83', // C no break here
			IND = '\x84', // D index (deprecated)
			NEL = '\x85', // E next line
			SSA = '\x86', // F start of selected area
			ESA = '\x87', // G end of selected area
			HTS = '\x88', // H horizontal tab set
			HTJ = '\x89', // I horizontal tab justified
			VTS = '\x8A', // J vertical tab set
			PLD = '\x8B', // K partial line down
			PLU = '\x8C', // L partial line up
			RI  = '\x8D', // M reverse line feed
			SS2 = '\x8E', // N single shift two
			SS3 = '\x8F', // O single shift three
			DCS = '\x90', // P device control string
			PU1 = '\x91', // Q private use one
			PU2 = '\x92', // R private use two
			STS = '\x93', // S set transmit state
			CCH = '\x94', // T cancel char
			MW  = '\x95', // U message waiting
			SPA = '\x96', // V start of guarded area
			EPA = '\x97', // W end of guarded area
			SOS = '\x98', // X start of string
			SCI = '\x9A', // Z single character introducer
			CSI = '\x9B', // [ control sequence introducer
			ST  = '\x9C', // \ string terminator
			OSC = '\x9D', // ] operating system command
			PM  = '\x9E', // ^ private message
			APC = '\x9F', // _ application program command
		};
	};

	struct G1 : plane<'\xA0', '\xFF'>
	{
		enum code : char
		{
			SHY = '\xAD', // soft hyphen
		};
	};

	struct CSI : plane<'\x40', '\x6F'>
	{
		enum code : char
		{
			ICH  = '\x40', // @ insert char
			CUU  = '\x41', // A cursor up
			CUD  = '\x42', // B cursor down
			CUF  = '\x43', // C cursor forward
			CUB  = '\x44', // D cursor back
			CNL  = '\x45', // E cursor next line
			CPL  = '\x46', // F cursor previous line
			CHA  = '\x47', // G cursor horizontal absolute
			CUP  = '\x48', // H cursor position
			CHT  = '\x49', // I cursor horizontal tab
			ED   = '\x4A', // J erase in display
			EL   = '\x4B', // K erase in line
			IL   = '\x4C', // L insert line
			DL   = '\x4D', // M delete line
			EF   = '\x4E', // N erase in field
			EA   = '\x4F', // O erase in area
			DCH  = '\x50', // P delete char
			SSE  = '\x51', // Q
			CPR  = '\x52', // R current position report
			SU   = '\x53', // S scroll up
			SD   = '\x54', // T scroll down
			NP   = '\x55', // U next page
			PP   = '\x56', // V previous page
			CTC  = '\x57', // W cursor tab control
			ECH  = '\x58', // X erase char
			CVT  = '\x59', // Y cursor vertical tab
			CBT  = '\x5A', // Z cursor backward tab
			SRS  = '\x5B', // [ start reversed string
			PTX  = '\x5C', // \ parallel texts
			SDS  = '\x5D', // ] start directed string
			SIMD = '\x5E', // ^ select implicit move direction
			HPA  = '\x60', // ` horizontal position absolute
			HPR  = '\x61', // a horizontal position forward
			REP  = '\x62', // b repeat
			DA   = '\x63', // c device attributes
			VPA  = '\x64', // d vertical position absolute
			VPR  = '\x65', // e vertical position forward
			HVP  = '\x66', // f horizontal vertical position
			TBC  = '\x67', // g tabulation clear
			SM   = '\x68', // h set mode
			MC   = '\x69', // i media copy
			HPB  = '\x6A', // j horizontal position backward
			VPB  = '\x6B', // k vertical position backward
			RM   = '\x6C', // l reset mode
			SGR  = '\x6D', // m select graphic rendition
			DSR  = '\x6E', // n device status report
			DAQ  = '\x6F', // o define area qualification

			// SP intermediate

			SL   = '\x40', // @ scroll left
			SR   = '\x41', // A scroll right
			GSM  = '\x42', // B graphic size modification
			GSS  = '\x43', // C graphic size selection
			FNT  = '\x44', // D font selection
			TSS  = '\x45', // E thin space specification
			JFY  = '\x46', // F justify
			SPI  = '\x47', // G spacing increment
			QUAD = '\x48', // H quad
			SSU  = '\x49', // I select size unit
			PFS  = '\x4A', // J page format selection
			SHS  = '\x4B', // K select horizontal spacing
			SVS  = '\x4C', // L select vertical spacing
			IGS  = '\x4D', // M identify graphic subrepretoire
			HTSA = '\x4E', // N horizontal tab set absolute (deprecated)
			IDCS = '\x4F', // O identify device control string
			PPA  = '\x50', // P page position absolute
			PPR  = '\x51', // Q page position forward
			PPB  = '\x52', // R page position backward
			SPD  = '\x53', // S select presentation direction
			DTA  = '\x54', // T dimension text area
			SLH  = '\x55', // U set line home
			SLL  = '\x56', // V set line limit
			FNK  = '\x57', // W function key
			SPQR = '\x58', // X select print quality & rapidity
			SEF  = '\x59', // Y sheet eject & feed
			PEC  = '\x5A', // Z presentation expand or contract
			SSW  = '\x5B', // [ select space width
			SACS = '\x5C', // \ set added char separation
			SAPV = '\x5D', // ] select alternative presentation variants
			STAB = '\x5E', // ^ selective tab
			GCC  = '\x5F', // _ graphic character combination
			TATE = '\x60', // ` tab aligned trailing edge
			TALE = '\x61', // a tab aligned leading edge
			TAC  = '\x62', // b tab aligned centred
			TCC  = '\x63', // c tab centered on char
			TSR  = '\x64', // d tab stop remove
			SCO  = '\x65', // e set char orientation
			SRCS = '\x66', // f set reduced char separation
			SCS  = '\x67', // g set char spacing
			SLS  = '\x68', // h set line spacing
			SCP  = '\x6B', // k select char path
		};
	};

	struct ICF : plane<'\x60', '\x7E'>
	{
		enum code : char
		{
			DMI  = '\x60', // ` disable manual input
			INT  = '\x61', // a interrupt
			EMI  = '\x62', // b enable manual input
			RIS  = '\x63', // c reset to initial state
			CMD  = '\x64', // d coding method delimiter
			LS2  = '\x6E', // n locking-shift two
			LS3  = '\x6F', // o locking-shift three
			LS3R = '\x7C', // | locking-shift three right
			LS2R = '\x7D', // } locking-shift two right
			LS1R = '\x7E', // ~ locking-shift one right
		};
	};

	//
	// Device Modes
	//

	enum mode : int
	{
		GATM =  1, // guarded area transfer mode
		KAM  =  2, // keyboard action mode
		CRM  =  3, // control representation mode
		IRM  =  4, // insertion replacement mode
		SRTM =  5, // status report transfer mode
		ERM  =  6, // erasure mode
		VEM  =  7, // vertical/line editing mode
		BDSM =  8, // bi-directional support mode
		DCSM =  9, // device component select mode
		HEM  = 10, // horizontal/char editing mode
		PUM  = 11, // positioning unit mode
		SRM  = 12, // send/receive mode
		FEAM = 13, // format effector action mode
		FETM = 14, // format effector transfer mode
		MATM = 15, // multiple area transfer mode
		TTM  = 16, // transfer termination mode
		SATM = 17, // select area transfer mode
		TSM  = 18, // tab stop mode
		EBM  = 19, // editing boundary mode (deprecated)
		LFNL = 20, // line feed/new line mode (deprecated)
		GRCM = 21, // graphic rendition combination mode
		ZDM  = 22, // zero default mode
	};

	//
	// Parameter Codes
	//

	enum SGR : int
	{
		reset         =  0, // attributes off
		intense       =  1, // bold or increased intensity
		faint         =  2, // faint or decreased intensity
		italic        =  3, // sometimes treated as inverse
		underline     =  4,
		blink_slow    =  5, // less than 150 per minute
		blink_fast    =  6, // more than 150 per minute
		reverse       =  7, // swap fg and bg colors
		conceal       =  8,
		strike        =  9, // characters crossed out
		first_font    = 10,
		last_font     = 19,
		fraktur       = 20,
		underline2    = 21,
		intense_off   = 22, // neither intense nor faint
		italic_off    = 23,
		underline_off = 24, // neither single nor double
		blink_off     = 25, // neither slow nor rapid
		inverse_off   = 27,
		reveal        = 28, // conceal off
		strike_off    = 29,
		fg_black      = 30,
		fg_red        = 31,
		fg_green      = 32,
		fg_yellow     = 33,
		fg_blue       = 34,
		fg_magenta    = 35,
		fg_cyan       = 36,
		fg_white      = 37,
		fg            = 38,
		fg_off        = 39,
		bg_black      = 40,
		bg_red        = 41,
		bg_green      = 42,
		bg_yellow     = 43,
		bg_blue       = 44,
		bg_magenta    = 45,
		bg_cyan       = 46,
		bg_white      = 47,
		bg            = 48,
		bg_off        = 49,
		frame         = 51,
		encircle      = 52,
		overline      = 53,
		frame_off     = 54, // neither framed nor encircled
		overline_off  = 55,
		rightline     = 60,
		rightline2    = 61,
		leftline      = 62,
		leftline2     = 63,
		stress        = 64,
		ideogram_off  = 65,
	};

	enum CTC : int
	{
		set_char        = 0, // char tab stop set at active position
		set_line        = 1, // line tab stop set at active line
		clear_char      = 2, // clear char tab stop at active position
		clear_line      = 3, // clear line tab stop at active line
		clear_all_chars = 4, // clear all char tab stops on line
		clear_all       = 5, // clear all tab stops
		clear_all_lines = 6, // clear all line tab stops
	};

	enum DAQ
	{
		unprotected_and_unguarded =  0, 
		protected_and_guarded     =  1,
		graphic_char_input        =  2,
		numeric_input             =  3,
		alphabetic_input          =  4,
		input_aligned_on_last     =  5,
		fill_with_zero            =  6,
		set_tab_stop              =  7,
		protected_and_unguarded   =  8,
		fill_with_space           =  9,
		input_aligned_on_first    = 10,
		reverse_input_order       = 11,
	};

	enum DSR
	{
		ready         = 0, // no malfunction
		busy_request  = 1, // must request later
		busy_reply    = 2, // will reply later
		error_request = 3, // malfunction, must request later
		error_reply   = 4, // malfunction, will reply later
		request       = 5, // request a DSR
		report        = 6, // request a CPR
	};

	enum EA
	{
		erase_to_area_end     = 0, // erase from active to end of area
		erase_from_area_begin = 1, // erase from beginning of area to active
		erase_area            = 2, // erase all in area
	};

	enum ED
	{
		erase_to_page_end     = 0, // erase from active to end of page
		erase_from_page_begin = 1, // erase form beginning of page to active
		erase_page            = 2, // erase all in page
	};

	enum EF
	{
		erase_to_field_end     = 0, // erase from active to end of field
		erase_from_field_begin = 1, // erase form beginning of field to active
		erase_field            = 2, // erase all in field
	};

	enum EL
	{
		erase_to_line_end     = 0, // erase from active to end of line
		erase_from_line_begin = 1, // erase form beginning of line to active
		erase_line            = 2, // erase all in line
	};

	enum JFY
	{
		none                = 0,
		word_fill           = 1, 
		word_space          = 2,
		letter_space        = 3,
		hyphenation         = 4,
		flush_to_home       = 5,
		center              = 6,
		flush_to_limit      = 7,
		italian_hyphenation = 8,
	};

	enum MC
	{
		to_primary               = 0,
		from_primary             = 1,
		to_secondary             = 2,
		from_seconary            = 3,
		stop_relay_to_primary    = 4,
		start_relay_to_primary   = 5,
		stop_relay_to_secondary  = 6,
		start_relay_to_secondary = 7,
	};

	enum PEC
	{
		normal    = 0, // as specified by SCS, SHS, or SPI
		expanded  = 1, // multiplied by a factor not greater than 2
		condensed = 2, // multiplied by a factor not less than 1/2
	};

	enum PFS
	{
		tall_basic_text  =  0,
		wide_basic_text  =  1,
		tall_basic_A4    =  2,
		wide_basic_A4    =  3,
		tall_letter      =  4,
		wide_letter      =  5,
		tall_extended_A4 =  6,
		wide_extended_A4 =  7,
		tall_legal       =  8,
		wide_legal       =  9,
		A4_short_lines   = 10,
		A4_long_lines    = 11,
		B5_short_lines   = 12,
		B5_long_lines    = 13,
		B4_short_lines   = 14,
		B4_long_lines    = 15,
	};

	enum PTX
	{
		end_parallel_text            = 0,
		begin_principal              = 1,
		begin_supplementary          = 2,
		Japanese_phonetic_annotation = 3,
		Chinese_phonetic_annotation  = 4,
		end_phonetic_annotations     = 5,
	};

	/*enum QUAD
	{
		flush_to_home            = 0,
		flush_to_home_with_fill  = 1,
		center                   = 2,
		center_with_fill         = 3,
		flush_to_limit           = 4,
		flush_to_limit_with_fill = 5,
		flush_to_both            = 6,
	};*/

	//
	// Utility Functions
	//

	using inter = plane<'\x20', '\x2F'>;
	using param = plane<'\x30', '\x3F'>;
	using close = plane<'\x40', '\x7E'>;
	using privy = plane<'\x70', '\x7E'>;

	namespace io
	{
		template <size_t N, int...P>
		struct set : std::array<int, N + sizeof...(P)>
		{
			set(std::span<int, N> par) : array(P...)
			{
				auto it = std::next(begin(), sizeof...(P));
				std::copy(par.begin(), par.end(), it);
			}
		};
		
		using reset = set<0, SGR::reset>;
		using intense = set<0, SGR::intense>;
		using faint = set<0, SGR::faint>;
		using italic = set<0, SGR::italic>;
		using underline = set<0, SGR::underline>;
		using blink_slow = set<0, SGR::blink_slow>;
		using blink_fast = set<0, SGR::blink_fast>;
		using reverse = set<0, SGR::reverse>;
		using conceal = set<0, SGR::conceal>;
		using strike = set<0, SGR::strike>;
		using font = set<0, SGR::first_font + N>;
		using fraktur = set<0, SGR::fraktur>;
		using underline2 = set<0, SGR::underline2>;
		using intense_off = set<0, SGR::intense_off>;
		using italic_off = set<0, SGR::italic_off>;
		using underline_off = set<0, SGR::underline_off>;
		using blink_off = set<0, SGR::blink_off>;
		using inverse_off = set<0, SGR::inverse_off>;
		using reveal = set<0, SGR::reveal>;
		using strike_off = set<0, SGR::strike_off>;
		using fg_black = set<0, SGR::fg_black>;
		using fg_red = set<0, SGR::fg_red>;
		using fg_green = set<0, SGR::fg_green>;
		using fg_yellow = set<0, SGR::fg_yellow>;
		using fg_blue = set<0, SGR::fg_blue>;
		using fg_magenta = set<0, SGR::fg_magenta>;
		using fg_cyan = set<0, SGR::fg_cyan>;
		using fg_white = set<0, SGR::fg_white>;
		using fg_rgb = set<3, SGR::fg, 2>;
		using fg = set<1, SGR_fg, 5>;
		using fg_off = set<0, SGR::fg_off>;
		using bg_black = set<0, SGR::bg_black>;
		using bg_red = set<0, SGR::bg_red>;
		using bg_green = set<0, SGR::bg_green>;
		using bg_yellow = set<0, SGR::bg_yellow>;
		using bg_blue = set<0, SGR::bg_blue>;
		using bg_magenta = set<0, SGR::bg_magenta>;
		using bg_cyan = set<0, SGR::bg_cyan>;
		using bg_white = set<0, SGR::bg_white>;
		using bg_rgb = set<3, SGR::bg, 2>;
		using bg = set<1, SGR::bg, 5>;
		using bg_off = set<0, SGR::bg_off>;
		using frame = set<0, SGR::frame>;
		using encircle = set<0, SGR::encircle>;
		using overline = set<0, SGR::overline>;
		using frame_off = set<0, SGR::frame_off>;
		using overline_off = set<0, SGR::overline_off>;
	}
}

template <size_t N, int...P>
fmt::string::out::ref operator<<(fmt::string::out::ref out, fmt::io::set<N, P...> par)
{
	return fmt::string::stream::put(out, par.begin(), par.end(), ";");
}

#endif // file
