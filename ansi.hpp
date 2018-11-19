#ifndef ANSI_hpp
#define ANSI_hpp

namespace ecma
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
		HT  = '\x09', // horizontal tab
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

		// ESC sequences

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

		// CSI sequences

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
		IL   = '\x4C', // L inert line
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
		               // N
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