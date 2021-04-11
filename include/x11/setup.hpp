#ifndef xsetup_hpp
#define xsetup_hpp

#include "x11.hpp"

namespace x11
{
	using ClientPrefix = Protocol
	<
		xConnClientPrefix, sz_xConnClientPrefix
	>;

	using SetupPrefix = Protocol
	<
		xConnSetupPrefix, sz_xConnSetupPrefix
	>;

	using Setup = Protocol
	<
		xConnSetup, sz_xConnSetup
	>;

	using PixmapFormat = Protocol
	<
		xPixmapFormat, sz_xPixmapFormat
	>;

	using Depth = Protocol
	<
		xDepth, sz_xDepth
	>;

	using VisualType = Protocol
	<
		xVisualType, sz_xVisualType
	>;

	using WindowRoot = Protocol
	<
		xWindowRoot, sz_xWindowRoot
	>;
}

#endif
