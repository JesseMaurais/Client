#ifndef sax_hpp
#define sax_hpp

#include "tag.hpp"

namespace xml
{
	template <class Char>
	constexpr fmt::basic_string_view_pair<Char> tags()
	{
		fmt::basic_string<Char> const first(1, (Char) '<');
		fmt::basic_string<Char> const second(1, (Char) '>');
		return fmt::basic_string_view_pair<Char>(first, second);
	}

	template <class Char> struct basic_sax
	{
		using char_type = typename Char;
		using string = fmt::basic_string<char_type>;
		using string_view = fmt::basic_string_view<char_type>;
		using string_view_pair = fmt::basic_string_view_pair<char_type>;
		using element_parser = tag::basic_parser<char_type>;
		using element_iterator = element_parser::iterator;

		basic_sax(string_view input)
		: element_parser(input, tags())
		{ }

		iterator begin()
		{
			auto it = parser.begin();
			return iterator(it);
		}

		iterator end()
		{
			auto it = parser.end();
			return iterator(it);
		}

		struct iterator
		{
			friend struct basic_sax;

			bool operator!=(iterator const& it) const
			{
				return it.element != element;
			}

			iterator operator++()
			{
				++element;
				return *this;
			}

		private:

			element_iterator element;

			iterator(element_iterator other)
			: element(other)
			{ }
		};

	private:

		element_parser parser;
	};

	using sax = basic_sax<char>;
	using wsax = basic_sax<wchar_t>;
}

#endif // file
