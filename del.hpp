#ifndef del_hpp
#define del_hpp

#include "str.hpp"

namespace fmt
{
	template <class Char>
	class basic_delimiter : protected basic_string_view_pair<Char>
	{
		using string_view = fmt::basic_string_view<Char>;
		using string_view_pair = fmt::basic_string_view_pair<Char>;

	private:

		string_view const view;

	public:

		basic_delimiter(string_view input, string_view_pair pair)
		: string_view_pair(pair), view(input)
		{ }

		basic_delimiter(string_view input, string_view mark)
		: basic_delimiter(input, { mark, mark })
		{ }

		class iterator : public size_pair
		{
			friend class basic_delimiter;
			basic_delimiter const* that;

			iterator(basic_delimiter const* owner, size_type pos)
			: size_pair(pos, pos), that(owner)
			{
				operator++();
			}

		public:

			size_pair operator*() const
			{
				return *this;
			}

			auto operator++()
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
		using span_view = basic_string_view_span<char_type>;

	protected:

		string_view view;
		span_view ring;

	public:

		basic_sequence(string_view input, span_view sequence)
		: view(input), ring(sequence)
		{ }

		class iterator : protected delimiter
		{
			friend class basic_sequence;
			basic_sequence const* that;

			typename delimiter::iterator mark;
			typename span_view::const_iterator tag;
	
			iterator(basic_sequence const* owner, bool start)
			: delimiter(owner->view, {owner->ring.back(), owner->ring.front()})
			, that(owner)
			, mark(start ? this->begin() : this->end())
			, tag(owner->ring.begin())
			{ }

		public:

			template <class C>
			size_type start_after(fmt::basic_string_view<C> s)
			{
				auto const m = s.size();
				auto const n = this->second - this->first;
				if (m < n) return npos;

				for (size_type i = 0; i < m; ++i)
				{
					if (s[i] != view[this->first + i])
					{
						return npos;
					}
				}
				return this->first + m;
			}

			template <class C>
			size_type ends_before(fmt::basic_string_view<C> s)
			{
				auto const m = s.size();
				auto const n = this->second - this->first;
				if (m < n) return npos;

				for (size_type i = 0; i < m; ++i)
				{
					if (s[i] != view[this->second - m + i])
					{
						return npos;
					}
				}
				return this->second - m;
			}
			
			operator span_view() const
			{
				return *tag;
			}

			size_pair operator*() const
			{
				return *mark;
			}

			bool operator!=(iterator const& it) const
			{
				return that != it.that or mark != it.mark;  
			}

			auto operator++()
			{
				this->first = *tag;
				++ tag;
				if (that->ring.end() == tag)
				{
					tag = that->ring.begin();
				}
				this->second = *tag;
				++ mark;
				return *this;
			}
		};

		auto begin() const
		{
			return iterator(this, true);
		}

		auto end() const
		{
			return iterator(this, false);
		}
	};

	using sequence = basic_sequence<char>;
	using wsequence = basic_sequence<wchar_t>;
}

#endif // file
