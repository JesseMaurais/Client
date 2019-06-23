#ifndef tag_hpp
#define tag_hpp

#include "str.hpp"

namespace fmt
{
	//
	// Iterator over occurances of a string inside another string
	//

	template <class Char> class basic_delimiter
	{
		using char_type = Char;
		using string_view = fmt::basic_string_view<char_type>;
		using string_size_pair = fmt::basic_string_size_pair<char_type>;
		using size_type = typename string_view::size_type;
		static constexpr auto npos = string_view::npos;

		string_view const view;
		string_view const mark;

	public:

		basic_delimiter(string_view input, string_view delimiter)
		: view(input), mark(delimiter)
		{ }

		friend class iterator;
		class iterator
		{
			friend class basic_delimiter;
			using string_size_pair = fmt::basic_string_size_pair<char_type>;
			using size_type = typename string_view::size_type;

			basic_delimiter const* that;
			string_size_pair pos;

			iterator(basic_delimiter const* owner, size_type start)
			: that(owner), pos(start, start)
			{
				operator++();
			}

		public:

			string_size_pair operator*() const
			{
				return pos;
			}

			string_size_pair const* operator->() const
			{
				return &pos;
			}

			bool operator<(iterator const& it) const
			{
				return that == it.that and pos.first < it.pos.first;
			}

			bool operator==(iterator const& it) const
			{
				return that == it.that and pos == it.pos;
			}

			bool operator!=(iterator const& it) const
			{
				return not operator==(it);
			}

			iterator operator++()
			{
				pos.first = that->view.find(that->mark, pos.second);
				pos.second = pos.first < that->view.size()
				           ? pos.first + that->mark.size()
				           : npos;

				return *this;
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
	};

	using delimiter = basic_delimiter<char>;
	using wdelimiter = basic_delimiter<wchar_t>;

	//
	// Iterator around tags delimited by open and closing strings
	//

	template <class Char> class basic_parser
	{
		using char_type = Char;
		using string_view = fmt::basic_string_view<char_type>;
		using string_view_pair = fmt::basic_string_view_pair<char_type>;
		using size_type = typename string_view::size_type;
		static constexpr auto npos = string_view::npos;

		using delimiter_type = basic_delimiter<char_type>;
		using iterator_pair = fmt::pair<typename delimiter_type::iterator>;

		fmt::pair<delimiter_type> delimiters;

	public:

		basic_parser(string_view input, string_view_pair marks)
		: delimiters(std::make_pair(
				delimiter_type(input, marks.first),
				delimiter_type(input, marks.second)
			))
		{ }

		class iterator
		{
			friend class basic_parser;

			basic_parser const* that;
			iterator_pair pos;

			iterator(basic_parser const* owner, iterator_pair const& marks)
			: that(owner), pos(std::make_pair(marks.first, marks.second))
			{ }

		public:

			bool operator!=(iterator const& it) const
			{
				return pos != it.pos and that != it.that;
			}

			string_size_pair operator*() const
			{
				if (*pos.first < *pos.second)
				{
					return string_size_pair(pos.first->second, pos.second->first);
				}
				else // second < first
				{
					return string_size_pair(pos.second->first, pos.first->second);
				}
			}

			iterator operator++()
			{
				if (*pos.first < *pos.second)
				{
					++pos.first;
				}
				else // second < first
				{
					++pos.second;
				}
				return *this;
			}
		};

		iterator begin() const
		{
			return iterator(this, std::make_pair(
				delimiters.first.begin(), delimiters.second.begin()
			));
		}

		iterator end() const
		{
			return iterator(this, std::make_pair(
				delimiters.first.end(), delimiters.second.end()
			));
		}
	};

	using parser = basic_parser<char>;
	using wparser = basic_parser<wchar_t>;
}

#endif // file
