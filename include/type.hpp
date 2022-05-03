#ifndef type_hpp
#define type_hpp "Character Types"

#include "fmt.hpp"
#include "it.hpp"
#include <locale>
#include <cmath>

namespace fmt
{
	const auto
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

	template <class Char> struct type : std::ctype<Char>
	{
		using string     = basic_string<Char>;
		using basic_type = std::ctype<Char>;
		using mask       = typename basic_type::mask;
		using view       = typename string::view;
		using init       = typename view::init;
		using span       = typename view::span;
		using pair       = typename view::pair;
		using vector     = typename view::vector;
		using ctype      = typename view::ctype;
		using iterator   = typename view::iterator;
		using pointer    = typename view::const_pointer;
		using size_type  = typename view::size_type;
		using size_pair  = fwd::pair<size_type>;
		using marks       = fwd::vector<mask>;

		bool check(Char c, mask x = space) const;
		// Check whether code $c is an $x

		marks check(view u) const;
		// Classify all characters in $u

		iterator next(iterator it, iterator end, mask x = space) const;
		// Next iterator after $it but before $end which is an $x

		iterator next(view u, mask x = space) const;
		// First character in view $u which is an $x

		iterator skip(iterator it, iterator end, mask x = space) const;
		// Next iterator after $it but before $end which is not $x

		iterator skip(view u, mask x = space) const;
		// First character in view $u which is not $x

		vector split(view u, mask x = space) const;
		// Split strings in $u delimited by $x

		iterator first(view u, mask x = space) const;
		// First iterator in view $u that is not $x

		iterator last(view u, mask x = space) const;
		// Last iterator in view $u that is not $x

		view trim(view u, mask x = space) const;
		// Trim $x off the front and back of $u

		bool all_of(view u, mask x = space) const;
		// All decoded characters in $u are $x

		bool any_of(view u, mask x = space) const;
		// Any decoded characters in $u are $x

		string to_upper(view u) const;
		// Recode characters in upper case

		string to_lower(view u) const;
		// Recode characters in lower case

		static pair to_pair(view u, view v);
		// Divide $u by first occurance of $v

		static bool terminated(view u);
		// Check whether string is null terminated

		static size_type count(view u, view v);
		// Count occurances in $u of a substring $v

		static string join(span t, view u);
		// Join strings in $t with $u inserted

		static vector split(view u, view v);
		// Split strings in $u delimited by $v

		static string replace(view u, view v, view w);
		// Replace in $u all occurrances of $v with $w

		static size_pair embrace(view u, view v = "()");
		// Position in $u with front and back braces in $v

		static type* get();
		// Current character type

		static type* set(type*);
		// Change current character type

		template <class C> static string from(const C &);
		// Indirection for calling `to_string` functions

	private:

		template <class It> It scan_is(It begin, It end, mask x) const;
		// Generic implementation of the base type's virtual method, scan_is

		template <class It> It scan_not(It begin, It end, mask x) const;
		// Generic implementation of the base type's virtual method, scan_not
	};

	//
	// Common characters
	//

	extern template struct type<char>;
	using ctype = type<char>;

	extern template struct type<wchar_t>;
	using wtype = type<wchar_t>;

	//
	// Multibyte conversions
	//

	template <class Char> auto length(Char u)
	// Size of UTF encoded data from first item
	{
		std::size_t n = 1;
		if (std::signbit(u))
		{
			for (n = 0; std::signbit(u << n); ++n);
		}
		return n;
	}

	inline auto widen(view u)
	// Decode UTF-8 string
	{
		struct iterator
		{
			const wtype* that;
			std::size_t size;
			const char* pos;

			bool operator!=(const iterator& it) const
			{
				return it.pos != pos;
			}

			auto operator*() const
			{
				wchar_t c[1];
				const auto size = length(*pos);
				(void) that->widen(pos, pos + size, c);
				return *c;
			}

			auto operator++()
			{
				pos += length(*pos);
				return *this;
			}

			iterator(const char* it)
			: that(wtype::get()), pos(it)
			{ }
		};

		const auto begin = u.data();
		const auto end = begin + u.size();
		return fwd::range<iterator>(begin, end);
	}

	inline auto narrow(wide w)
	// Encode UTF-8 string
	{
		struct iterator
		{
			const wtype* that;
			const wchar_t* pos;

			bool operator!=(iterator const& it) const
			{
				return it.pos != pos;
			}

			auto operator*() const
			{
				string s(MB_CUR_MAX, 0);
				(void) that->narrow(pos, pos + 1, 0, s.data());
				return s;
			}

			auto operator++()
			{
				++ pos;
				return *this;
			}

			iterator(const wchar_t* it)
			: that(wtype::get()), pos(it)
			{ }
		};

		const auto begin = w.data();
		const auto end = begin + w.size();
		return fwd::range<iterator>(begin, end);
	}

	//
	// Multibyte shims
	//

	inline auto next(view::iterator it, view::iterator end)
	{
		return ctype::get()->next(it, end);
	}

	inline auto next(view u)
	{
		return ctype::get()->next(u);
	}

	inline auto skip(view::iterator it, view::iterator end)
	{
		return ctype::get()->skip(it, end);
	}

	inline auto skip(view u)
	{
		return ctype::get()->skip(u);
	}

	inline auto first(view u)
	{
		return ctype::get()->first(u);
	}

	inline auto last(view u)
	{
		return ctype::get()->last(u);
	}

	inline auto trim(view u)
	{
		return ctype::get()->trim(u);
	}

	inline auto all_of(view u, ctype::mask m)
	{
		return ctype::get()->all_of(u, m);
	}

	inline auto any_of(view u, ctype::mask m)
	{
		return ctype::get()->any_of(u, m);
	}

	inline auto to_upper(view u)
	{
		return ctype::get()->to_upper(u);
	}

	inline auto to_lower(view u)
	{
		return ctype::get()->to_lower(u);
	}

	inline bool terminated(view u)
	{
		return ctype::get()->terminated(u);
	}

	inline auto count(view u, view v)
	{
		return ctype::get()->count(u, v);
	}

	inline auto join(view::span t, view u = empty)
	{
		return ctype::get()->join(t, u);
	}

	inline auto split(view u, ctype::mask m = space)
	{
		return ctype::get()->split(u, m);
	}

	inline auto split(view u, view v)
	{
		return ctype::get()->split(u, v);
	}

	inline auto replace(view u, view v, view w)
	{
		return ctype::get()->replace(u, v, w);
	}

	inline auto embrace(view u, view v)
	{
		return ctype::get()->embrace(u, v);
	}

	inline auto to_pair(view u, view v = assign)
	{
		return ctype::get()->to_pair(u, v);
	}

	inline auto to_pair(view::pair p, view u = assign)
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
	inline string to_string(T const& x)
	{
		return std::to_string(x);
	}

	template <typename T>
	inline wstring to_wstring(T const& x)
	{
		return std::to_wstring(x);
	}

	template <>
	inline string to_string(string const& s)
	{
		return s;
	}

	template <>
	inline wstring to_wstring(wstring const& w)
	{
		return w;
	}

	template <>
	inline string to_string(string::view const& s)
	{
		return fmt::string(s.data(), s.size());
	}

	template <>
	inline wstring to_wstring(wstring::view const& w)
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
	inline string to_string(char const& c)
	{
		return string(1, c);
	}

	template <>
	inline wstring to_wstring(wchar_t const& c)
	{
		return wstring(1, c);
	}

	//
	// Wide & narrow conversions
	//

	template <>
	inline string to_string(wstring::view const& w)
	{
		string s;
		for (auto const c : narrow(w)) s += c;
		return s;
	}

	template <>
	inline wstring to_wstring(string::view const& s)
	{
		wstring w;
		for (auto const c : widen(s)) w += c;
		return w;
	}

	template <>
	inline string to_string(wstring const& w)
	{
		return to_string(wstring::view(w));
	}

	template <>
	inline wstring to_wstring(string const& s)
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
	inline string to_string(wchar_t const& c)
	{
		return to_string(wstring(1, c));
	}

	template <>
	inline wstring to_wstring(char const& c)
	{
		return to_wstring(string(1, c));
	}

	// Converters for character class

	template <> template <class T> string type<char>::from(T const& s)
	{
		return ::fmt::to_string(s);
	}

	template <> template <class T> wstring type<wchar_t>::from(T const& s)
	{
		return ::fmt::to_wstring(s);
	}
}

#endif // file
