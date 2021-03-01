#include "x11.hpp"

namespace x11
{
	using Segment = Protocol<xSegment, sz_xSement>;
	using Point = Protocol<xPoint, sz_xPoint>;
	using Rectangle = Protocol<xRectangle, sz_xRectangle>;
	using Arc = Protocol<xArc, sz_xArc>;
	using TimeCoord = Protocol<xTimeCoord, sz_xTimeCoord>;
	using HostEntry = Protocol<xHostEntry, sz_xHostEntry>;
	using CharInfo = Protocol<xCharInfo, sz_xCharInfo>;
	using FontProp = Protocol<xFontProp, sz_xFontProp>;
};

