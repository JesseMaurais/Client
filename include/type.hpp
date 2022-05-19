#ifndef type_hpp
#define type_hpp "Character Types"

#include "fmt.hpp"
#include "it.hpp"
#include <locale>

namespace fmt
{
	inline auto
		space  = std::ctype_base::space,
		print  = std::ctype_base::print,
		cntrl  = std::ctype_base::cntrl,
		upper  = std::ctype_base::upper,
		lower  = std::ctype_base::lower,
		alpha  = std::ctype_base::alpha,
		digit  = std::ctype_base::digit,
		punct  = std::ctype_base::punct,
		blank  = std::ctype_base::blank,
		alnum  = std::ctype_base::alnum,
		graph  = std::ctype_base::graph,
		xdigit = std::ctype_base::xdigit;

	using mask = decltype(space);
	using mark = fwd::vector<mask>;

	template <class Char> struct type
	{
		using ctype      = std::ctype<Char>;
		using collate    = std::collate<Char>;
		using messages   = std::messages<Char>;
		using num_get    = std::num_get<Char>;
		using time_get   = std::time_get<Char>;
		using money_get  = std::money_get<Char>;
		using num_put    = std::num_put<Char>;
		using time_put   = std::time_put<Char>;
		using money_put  = std::money_put<Char>;
		using numpunct   = std::numpunct<Char>;
		using moneypunct = std::moneypunct<Char>;

		using string     = basic_string<Char>;
		using view       = typename string::view;
		using init       = typename view::init;
		using span       = typename view::span;
		using pair       = typename view::pair;
		using vector     = typename view::vector;
		using input      = typename view::in::ref;
		using output     = typename view::out::ref;
		using iterator   = typename view::iterator;
		using pointer    = typename view::const_pointer;
		using size_type  = typename view::size_type;
		using size_pair  = fwd::pair<size_type>;

		static bool check(Char c, mask x = space);
		// Check whether code $c is an $x

		static mark check(view u);
		// Classify all characters in $u

		static iterator next(iterator it, iterator end, mask x = space);
		// Next iterator after $it but before $end which is an $x

		static iterator next(view u, mask x = space);
		// First character in view $u which is an $x

		static iterator skip(iterator it, iterator end, mask x = space);
		// Next iterator after $it but before $end which is not $x

		static iterator skip(view u, mask x = space);
		// First character in view $u which is not $x

		static vector split(view u, mask x = space);
		// Split strings in $u delimited by $x

		static iterator first(view u, mask x = space);
		// First iterator in view $u that is not $x

		static iterator last(view u, mask x = space);
		// Last iterator in view $u that is not $x

		static view trim(view u, mask x = space);
		// Trim $x off the front and back of $u

		static bool all_of(view u, mask x = space);
		// All decoded characters in $u are $x

		static bool any_of(view u, mask x = space);
		// Any decoded characters in $u are $x

		static string to_upper(view u);
		// Recode characters in upper case

		static string to_lower(view u);
		// Recode characters in lower case

		static pair to_pair(view u, view v);
		// Divide $u by first occurrence of $v

		static bool terminated(view u);
		// Check whether string is null terminated

		static size_type count(view u, view v);
		// Count occurrences in $u of a substring $v

		static string join(span t, view u);
		// Join strings in $t with $u inserted

		static vector split(view u, view v);
		// Split strings in $u delimited by $v

		static string replace(view u, view v, view w);
		// Replace in $u all occurrances of $v with $w

		static size_pair embrace(view u, view v);
		// Position in $u with front and back braces in $v

		static size_type length(Char headbyte);
		// Find the multibyte length from first

		static const ctype* use_ctype();

		static auto widen(fmt::view u)
		{
			struct iterator
			{
				const ctype* that = use_ctype();
				const char* pos;
				size_type size;

				iterator(const char* it)
				: pos(it)
				{ }

				bool operator!=(const iterator& it) const
				{
					return it.pos != pos;
				}

				auto operator*() const
				{
					Char c[1];
					const auto size = length(*pos);
					(void) that->widen(pos, pos + size, c);
					return *c;
				}

				void operator++()
				{
					pos += length(*pos);
				}
			};

			const auto begin = u.data();
			const auto end = begin + u.size();
			return fwd::range<iterator>(begin, end);
		}

		static auto widen(fmt::wide w)
		{
			return w;
		}

		static auto narrow(view w)
		{
			struct iterator
			{
				const ctype* that = use_ctype();
				const Char* pos;

				iterator(const Char* it)
				: pos(it)
				{ }

				bool operator!=(const iterator& it) const
				{
					return it.pos != pos;
				}

				auto operator*() const
				{
					fmt::string s(MB_CUR_MAX, 0);
					constexpr auto size = sizeof(char);
					(void) that->narrow(pos, pos + size, 0, s.data());
					return s;
				}

				void operator++()
				{
					++ pos;
				}
			};

			const auto begin = w.data();
			const auto end = begin + w.size();
			return fwd::range<iterator>(begin, end);
		}

		template <class It> static It scan_is(It begin, It end, mask x);
		// Generic implementation of the base type's virtual method, scan_is

		template <class It> static It scan_not(It begin, It end, mask x);
		// Generic implementation of the base type's virtual method, scan_not
	};

	//
	// UTF-8 shims
	//

	inline auto next(view::iterator it, view::iterator end)
	{
		return type<char>::next(it, end);
	}

	inline auto next(view u)
	{
		return type<char>::next(u);
	}

	inline auto skip(view::iterator it, view::iterator end)
	{
		return type<char>::skip(it, end);
	}

	inline auto skip(view u)
	{
		return type<char>::skip(u);
	}

	inline auto first(view u)
	{
		return type<char>::first(u);
	}

	inline auto last(view u)
	{
		return type<char>::last(u);
	}

	inline auto trim(view u)
	{
		return type<char>::trim(u);
	}

	inline auto all_of(view u, mask m = space)
	{
		return type<char>::all_of(u, m);
	}

	inline auto any_of(view u, mask m = space)
	{
		return type<char>::any_of(u, m);
	}

	inline auto to_upper(view u)
	{
		return type<char>::to_upper(u);
	}

	inline auto to_lower(view u)
	{
		return type<char>::to_lower(u);
	}

	inline bool terminated(view u)
	{
		return type<char>::terminated(u);
	}

	inline auto count(view u, view v)
	{
		return type<char>::count(u, v);
	}

	inline string join(span t, view u = tag::empty)
	{
		return type<char>::join(t, u);
	}

	inline auto split(view u, mask m = space)
	{
		return type<char>::split(u, m);
	}

	inline auto split(view u, view v)
	{
		return type<char>::split(u, v);
	}

	inline auto replace(view u, view v, view w)
	{
		return type<char>::replace(u, v, w);
	}

	inline auto embrace(view u, view v)
	{
		return type<char>::embrace(u, v);
	}

	inline auto to_pair(view u, view v = tag::assign)
	{
		return type<char>::to_pair(u, v);
	}

	inline auto to_pair(view::pair p, view u = tag::assign)
	{
		view::vector x { p.first, p.second };
		return fmt::join(x, u);
	}

	inline bool same(view u, view v)
	{
		return u.empty() ? v.empty() :
			u.data() == v.data() and u.size() == v.size();
	}

	//
	// Generic string converters
	//

	template <typename T>
	inline string to_string(const T& x)
	{
		return std::to_string(x);
	}

	template <typename T>
	inline wstring to_wstring(const T& x)
	{
		return std::to_wstring(x);
	}

	template <>
	inline string to_string(const string& s)
	{
		return s;
	}

	template <>
	inline wstring to_wstring(const wstring& w)
	{
		return w;
	}

	template <>
	inline string to_string(const view& s)
	{
		return string(s.data(), s.size());
	}

	template <>
	inline wstring to_wstring(const wide& w)
	{
		return wstring(w.data(), w.size());
	}

	template <>
	inline string to_string(char const*const& s)
	{
		return s ? string(s) : string();
	}

	template <>
	inline wstring to_wstring(wchar_t const*const& w)
	{
		return w ? wstring(w) : wstring();
	}

	template <>
	inline string to_string(char *const& s)
	{
		return s ? string(s) : string();
	}

	template <>
	inline wstring to_wstring(wchar_t *const& w)
	{
		return w ? wstring(w) : wstring();
	}

	template <>
	inline string to_string(const char& c)
	{
		return string(1, c);
	}

	template <>
	inline wstring to_wstring(const wchar_t& c)
	{
		return wstring(1, c);
	}

	//
	// Wide & narrow conversions
	//

	template <>
	inline string to_string(const wide& w)
	{
		string s;
		for (auto c : type<wchar_t>::narrow(w)) s += c;
		return s;
	}

	template <>
	inline wstring to_wstring(const view& s)
	{
		wstring w;
		for (auto c : type<char>::widen(s)) w += c;
		return w;
	}

	template <>
	inline string to_string(const wstring& w)
	{
		return to_string(wstring::view(w));
	}

	template <>
	inline wstring to_wstring(const string& s)
	{
		return to_wstring(string::view(s));
	}

	template <>
	inline string to_string(wchar_t const*const& w)
	{
		return to_string(wstring::view(w));
	}

	template <>
	inline wstring to_wstring(char const*const& s)
	{
		return to_wstring(string::view(s));
	}

	template <>
	inline string to_string(wchar_t *const& w)
	{
		return to_string(wstring::view(w));
	}

	template <>
	inline wstring to_wstring(char *const& s)
	{
		return to_wstring(string::view(s));
	}

	template <>
	inline string to_string(const wchar_t& c)
	{
		return to_string(wstring(1, c));
	}

	template <>
	inline wstring to_wstring(const char& c)
	{
		return to_wstring(string(1, c));
	}
}

#endif // file
