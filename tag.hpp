#ifndef tag_hpp
#define tag_hpp

#include "str.hpp"

namespace tag
{
	template <class Char> struct basic_parser
	{
		using char_type = Char;
		using string = fmt::basic_string<char_type>;
		using string_view = fmt::basic_string_view<char_type>;
		using string_size_pair = fmt::basic_string_size_pair<char_type>;
		using size_type = string_view::size_type;

		struct iterator
		{
			friend struct basic_parser;

			bool operator!=(iterator const& it) const
			{
				return it.pos != pos or it.mark != mark or not same(it.view, view);
			}

			string_size_pair operator*() const
			{
				return pos;
			}

			iterator operator++()
			{
				iterator(pos.second, mark.second);
				iterator(pos.first, mark.first);
				return *this;
			}

		private:

			iterator(string_view input, string_view_pair marks, string_size_pair sizes)
			: view(input), mark(marks), pos(sizes)
			{
				iterate(pos.first, mark.first);
			}

			string_view const view;
			string_view_pair const mark;
			string_size_pair pos;
			size_type next = 0;

			void iterate(size_type& position, string_view marker)
			{
				position = view.find(marker, next);
				next = step(position, marker);
			}

			size_type step(size_type n, string_view u)
			{
				return n < view.size() ? n + u.size() : view.size();
			}
		};

		iterator begin() const
		{
			return iterator(view, mark, { 0, 0 });
		}

		iterator end() const
		{
			return iterator(view, mark, { npos, npos });
		}

		basic_parser(string_view input, string_view_pair marks)
		: view(input), mark(marks)
		{ }

	private:

		string_view const view;
		string_view_pair const mark;
	};

	using parser = basic_parser<char>;
	using wparser = basic_parser<wchar_t>;
}

#endif // file
