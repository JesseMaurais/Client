#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include "fmt.hpp"

namespace doc
{
	template <class C> struct brief : fmt::struct_brief<C>
	{
		using string = fmt::string;
		using line   = string::line;
		using init   = string::init;
		using view   = string::view;
		using span   = string::span;
		using vector = string::vector;
		using out    = string::out;
		using in     = string::in;
	};

	template <class C> auto const& table(C* = nullptr)
	{
		return C::table;
	}
}

#endif // file
