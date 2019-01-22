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
				return it.next != next or it.mark != mark or not same(it.view, view);
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

			string_view const view;
			string_view_pair const mark;
			string_size_pair step { 0, 0 };
			size_type next;

			iterator(string_view input, string_view_pair stops, string_size_pair start)
			: view(input), mark(stops), next(start)
			{
				iterate();
			}

			void iterate()
			{
				step.first = step_to(mark.first);
				step.second = step_to(mark.second);
			}

			size_type step_to(string_view marker)
			{
				size_type const position = view.find(marker, next);
				next = step_over(position, marker);
				return position;
			}

			size_type step_over(size_type n, string_view u)
			{
				return n < view.size() ? n + u.size() : npos;
			}
		};

		iterator begin() const
		{
			return iterator(view, mark, 0);
		}

		iterator end() const
		{
			return iterator(view, mark, npos);
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
