#include "txt.hpp"
#include "fmt.hpp"
#include "pstream.hpp"
#include <cassert>

namespace
{
	namespace impl
	{
		using namespace fmt;
		template <class Char>
		basic_string_view<Char> get(basic_string_view<Char> text)
		{
			assert(terminated(text));
			static thread_local string_view<Char> buf;
			sys::io::basic_ipstream<Char> ip { "gettext", text.c_str() };
			if (not std::getline(ip, buf)) buf.clear();
			return buf;
		}
	}
}

namespace txt
{
	string_view get(string_view text)
	{
		return ::impl::get(text); 
	}

	wstring_view get(wstring_view text)
	{
		return ::impl::get(text);
	}
}

