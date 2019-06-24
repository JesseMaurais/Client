#ifndef tag_hpp
#define tag_hpp

#include "str.hpp"

namespace fmt
{
	template <class Char> class basic_delimiter : public basic_string_view_pair<Char>
	{
		using char_type = Char;
		using string_view = basic_string_view<char_type>;
		using string_size_pair = basic_string_size_pair<char_type>;
		using string_view_pair = basic_string_view_pair<char_type>;
		using size_type = typename string_view::size_type;
		static constexpr auto npos = string_view::npos;

	private:

		string_view const view;

	public:

		basic_delimiter(string_view input, string_view_pair pair)
		: string_view_pair(pair), view(input)
		{ }

		basic_delimiter(string_view input, string_view mark)
		: basic_delimiter(input, {mark, mark})
		{ }

		class iterator : public string_size_pair
		{
			friend class basic_delimiter;
			basic_delimiter const* that;

			iterator(basic_delimiter const* owner, size_type start)
			: string_size_pair(start, start), that(owner)
			{
				operator++();
			}

		public:

			string_size_pair operator*() const
			{
				return *this;
			}

			iterator operator++()
			{
				this->first = this->second;
				if (this->first < npos)
				{
					if (0 < this->second)
					{
						this->first += that->first.size();
					}
					this->second = that->view.find(that->second, this->first);
				}
				return *this;
			}
		};

		auto begin() const
		{
			return iterator(this, 0);
		}

		auto end() const
		{
			return iterator(this, npos);
		}
	};

	using delimiter = basic_delimiter<char>;
	using wdelimiter = basic_delimiter<wchar_t>;


	template <class Char> class basic_sequence
	{
		using char_type = Char;
		using delimiter = basic_delimiter<char_type>;
		using string_view = basic_string_view<char_type>;
		using span_view = basic_span_view<char_type>;

	protected:

		string_view view;
		span_view ring;

	public:

		basic_sequence(string_view input, span_view sequence)
		: view(input), ring(sequence)
		{ }

		class iterator
		{
			friend class basic_sequence;
			basic_sequence const* that;
			delimiter marks;

			typename delimiter::iterator it;
			typename span_view::const_iterator tag;
	
			iterator(basic_sequence const* owner, bool end)
			: that(owner)
			, marks(owner->view, {owner->ring.back(), owner->ring.front()})
			, it(end ? marks.end() : marks.begin())
			, tag(owner->ring.begin())
			{ }

		public:

			string_size_pair operator*() const
			{
				return *it;
			}

			bool operator!=(iterator const& other) const
			{
				return that != other.that or it != other.it;  
			}

			iterator operator++()
			{
				marks.first = *tag;
				++ tag;
				if (that->ring.end() == tag)
				{
					tag = that->ring.begin();
				}
				marks.second = *tag;
				++ it;
				return *this;
			}
		};

		auto begin() const
		{
			return iterator(this, false);
		}

		auto end() const
		{
			return iterator(this, true);
		}
	};

	using sequence = basic_sequence<char>;
	using wsequence = basic_sequence<wchar_t>;
}

#endif // file
