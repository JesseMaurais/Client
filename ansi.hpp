#ifndef ANSI_hpp
#define ANSI_hpp

namespace ansi
{
	enum class code : char
	{
		esc = '\x1B', // escape
		ss2 = '\x8E', // N single shift two
		ss3 = '\x8F', // O single shift three
		dcs = '\x90', // P device control string
		csi = '\x9B', // [ control sequence introducer
		st  = '\x9C', // \ string terminator
		osc = '\x9D', // ] operating system command
		sos = '\x98', // X start of string
		pm  = '\x9E', // ^ private message
		apc = '\x9F', // _ application program command
		ris = '\x63', // c reset to initial state

		// CSI sequences

		cuu = '\x41', // A cursor up
		cud = '\x42', // B cursor down
		cuf = '\x43', // C cursor forward
		cub = '\x44', // D cursor back
		cnl = '\x45', // E cursor next line
		cpl = '\x46', // F cursor previous line
		cha = '\x47', // G cursor horizontal absolute
		cup = '\x48', // H cursor position
		ed  = '\x49', // J erase in display
		el  = '\x4A', // K erase in line
		su  = '\x53', // S scroll up
		sd  = '\x54', // T scroll down
		hvp = '\x66', // f horizontal vertical position
		sgr = '\x6D', // m select graphic rendition
		scp = '\x73', // s save cursor position
		rcp = '\x75', // u restore cursor position
	};

	enum class mode : int
	{
		reset = 0,
		intense = 1,
		faint = 2,
		italic = 3,
		underline = 4,
		blink_slow = 5,
		blink_rapid = 6,
		reverse = 7,
		conceal = 8,
		strike = 9,
		first_font = 10,
		last_font = 19,
		fraktur = 20,
		doubleline = 21,
		intense_off = 22,
		italic_off = 23,
		underline_off = 24,
		blink_off = 25,
		inverse_off = 27,
		reveal = 28,
		strike_off = 29,
		black_fg = 30,
		red_fg = 31,
		green_fg = 32,
		yellow_fg = 33,
		blue_fg = 34,
		magenta_fg = 35,
		cyan_fg = 36,
		white_fg = 37,
		fg = 38,
		fg_off = 39,
		black_bg = 40,
		red_bg = 41,
		green_bg = 42,
		yellow_bg = 43,
		blue_bg = 44,
		magenta_bg = 45,
		cyan_bg = 46,
		white_bg = 47,
		bg = 48,
		bg_off = 49,
		frame = 51,
		encircle = 52,
		overline = 53,
		frame_off = 54,
		overline_off = 55,
	};

	constexpr mode cycle(mode op)
	{
		switch (op)
		{
			case mode::intense:
				return mode::intense_off;
			case mode::faint:
				return mode::intense;
			case mode::intense_off:
				return mode::faint;

			case mode::blink_off:
				return mode::blink_slow;
			case mode::blink_slow:
				return mode::blink_rapid;
			case mode::blink_rapid:
				return mode::blink_off;

			case mode::strike:
				return mode::strike_off;
			case mode::strike_off:
				return mode::strike;

			default:
				return op;
		}
	}
}

#endif // file 