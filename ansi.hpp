#ifndef ANSI_hpp
#define ANSI_hpp

namespace ansi
{
	enum class code : char
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
		TAB = '\x09', // horizontal tab
		LF  = '\x0A', // line feed/new line
		VT  = '\x0B', // vertical tab
		FF  = '\x0C', // form feed/new page
		CR  = '\x0D', // carriage return
		SO  = '\x0E', // shift out
		SI  = '\x0F', // shift in
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
		SPC = '\x20', // space
		DEL = '\x7F', // delete

		SS2 = '\x8E', // N single shift two
		SS3 = '\x8F', // O single shift three
		DCS = '\x90', // P device control string
		CSI = '\x9B', // [ control sequence introducer
		ST  = '\x9C', // \ string terminator
		OSC = '\x9D', // ] operating system command
		SOS = '\x98', // X start of string
		PM  = '\x9E', // ^ private message
		APC = '\x9F', // _ application program command
		RIS = '\x63', // c reset to initial state

		// CSI sequences

		CUU = '\x41', // A cursor up
		CUD = '\x42', // B cursor down
		CUF = '\x43', // C cursor forward
		CUB = '\x44', // D cursor back
		CNL = '\x45', // E cursor next line
		CPL = '\x46', // F cursor previous line
		CHA = '\x47', // G cursor horizontal absolute
		CUP = '\x48', // H cursor position
		ED  = '\x49', // J erase in display
		EL  = '\x4A', // K erase in line
		SU  = '\x53', // S scroll up
		SD  = '\x54', // T scroll down
		HVP = '\x66', // f horizontal vertical position
		SGR = '\x6D', // m select graphic rendition
		SCP = '\x73', // s save cursor position
		RCP = '\x75', // u restore cursor position
	};

	enum class mode : int
	{
		reset         =  0, // attributes off
		intense       =  1, // bold or increased intensity
		faint         =  2, // faint or decreased intensity
		italic        =  3, // sometimes treated as inverse
		underline     =  4,
		blink_slow    =  5, // less than 150 per minute
		blink_rapid   =  6, // more than 150 per minute
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
		blink_off     = 25,
		inverse_off   = 27,
		reveal        = 28, // conceal off
		strike_off    = 29,
		black_fg      = 30,
		red_fg        = 31,
		green_fg      = 32,
		yellow_fg     = 33,
		blue_fg       = 34,
		magenta_fg    = 35,
		cyan_fg       = 36,
		white_fg      = 37,
		fg            = 38,
		fg_off        = 39,
		black_bg      = 40,
		red_bg        = 41,
		green_bg      = 42,
		yellow_bg     = 43,
		blue_bg       = 44,
		magenta_bg    = 45,
		cyan_bg       = 46,
		white_bg      = 47,
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
}

#endif // file 