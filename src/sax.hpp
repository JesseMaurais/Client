#ifndef sax_hpp
#define sax_hpp

#include "fmt.hpp"
#include "err.hpp"

namespace doc::sax
{
	fmt::view const braces = "<>";

	template <class Char, template <class> class Traits = std::char_traits>
	auto getline(fmt::view in)
	{
		fmt::view out = fmt::nil;
		auto const pos = fmt::embrace(in, braces);
		if (fmt::npos != pos.first)
		{
			out = in.substr(
		}
		return out;
	}
}

#endif // file
