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

		friend struct iterator
		{
			friend struct basic_parser;

			bool operator!=(iterator const& it) const
			{
				return it.next != next or it.that != that;
			}

			string_size_pair operator*() const
			{
				return step;
			}

			iterator operator++()
			{
				iterate();
				return *this;
			}

		private:

			basic_parser const* that;
			string_size_pair step { 0, 0 };
			size_type next;

			iterator(basic_parser const* parser, string_size_pair start)
			: that(parser), next(start)
			{
				iterate();
			}

			void iterate()
			{
				step.first = step_to(that->mark.first);
				step.second = step_to(that->mark.second);
			}

			size_type step_to(string_view mark)
			{
				size_type const pos = that->view.find(mark, next);
				next = step_over(mark.size(), pos);
				return pos;
			}

			size_type step_over(size_type m, size_type n) const
			{
				return n < that->view.size() ? n + m : npos;
			}
		};

		iterator begin() const
		{
			return iterator(this, 0);
		}

		iterator end() const
		{
			return iterator(this, npos);
		}

		basic_parser(string_view input, string_view_pair stops)
		: view(input), mark(stops)
		{ }

	private:

		string_view const view;
		string_view_pair const mark;
	};

	using parser = basic_parser<char>;
	using wparser = basic_parser<wchar_t>;
}

#endif // file
