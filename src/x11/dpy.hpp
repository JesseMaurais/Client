#ifndef xdpy_hpp
#define xdpy_hpp

#include "x11/setup.hpp"

namespace x11
{
	fmt::string setup
	(
		std::iostream & io, 
		fmt::string_view proto, 
		fmt::string_view string
	);

	struct Display : Setup
	{
		fmt::string vendor;
		WindowRoot::vector screens;
		PixmapFormat::vector formats;
		Depth::vector depths;
		VisualType::vector visuals;

		fmt::string setup(std::istream & in)
		{
			auto err = ::x11::setup();
			if (empty(err) and in)
			{
				vendor.resize(nbytesVendor);
				in.read(vendor.data(), nbyteVendor);
				formats.resize(numFormats);
				in >> formats;
				screens.resize(numRoots);
				in >> screens;
			}
			return err;
		}
	};
}

#endif
