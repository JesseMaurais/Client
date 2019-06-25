#ifndef sax_hpp
#define sax_hpp

#include "del.hpp"

namespace xml
{
	template <class Char> class basic_sax : public fmt::basic_sequence<Char>
	{
		using char_type = Char;
		using string = fmt::basic_string<char_type>;
		using string_view = fmt::basic_string_view<char_type>;
		using string_view_pair = fmt::basic_string_view_pair<char_type>;
		using string_vector = fmt::basic_string_vector<char_type>;
		using span_view = fmt::basic_span_view<char_type>;
		using sequence = fmt::basic_sequence<char_type>;
		using iterator = typename sequence::iterator;

	public:

		basic_sax(string_view view)
		: basic_sequence(view, { lt, gt })
		{ }

		static bool is_element(iterator const& it)
		{
			return gt == it;
		}

		static bool is_comment(iterator const& it)
		{
			return is_match("!--", it, "--");
		}

		static bool is_prolog(iterator const& it)
		{
			return is_match("?xml ", it, "?");
		}

	private:

		static bool is_match(char const* prefix, iterator const& it, char const* suffix)
		{
			return it.starts_with(prefix) and it.ends_with(suffix);
		}
	};

	using sax = basic_sax<char>;
	using wsax = basic_sax<wchar_t>;
}

#endif // file
