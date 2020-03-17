#include "x11.hpp"
#include "str.hpp"
#include <iterator>
#include <algorithm>

namespace x11
{
	template
	<
		class Structure, short Size
	>
	using Geometry = Protocol<Structure, Size>;
	using Segment = Geometry<xSegment, sz_xSement>;
	using Point = Geometry<xPoint, sz_xPoint>;
	using Rectangle = Geometry<xRectangle, sz_xRectangle>;
	using Arc = Geometry<xArc, sz_xArc>;
	using TimeCoord = Protocol<xTimeCoord, sz_xTimeCoord>;
	using HostEntry = Protocol<xHostEntry, sz_xHostEntry>;
	using CharInfo = Protocol<xCharInfo, sz_xCharInfo>;
	using FontProp = Protocol<xFontProp, sz_xFontProp>;
};

