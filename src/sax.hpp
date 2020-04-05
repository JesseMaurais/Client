#ifndef sax_hpp
#define sax_hpp

#include "fmt.hpp"
#include "err.hpp"

namespace doc::sax
{
	fmt::string_view const braces = "<>";

	template 
	<
		class Char,
		template <class> class Traits = std::char_traits,
		template <class, class> class View = fwd::basic_string_view
	>
	auto get(View<Char, Traits> in, View<Char, Traits> braces = { '<', '>'})
	{
		auto const pos = fmt::embrace(in, braces);
		View<Char, Traits> out;
		if (fmt::npos != pos.first)
		{
			out = in.substr(pos.first, pos.second);
		}
		return out;
	}
}

#endif // file
