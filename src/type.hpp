#ifndef type_hpp
#define type_hpp "Character Types"

#include "fmt.hpp"
#include "it.hpp"
#include "utf.hpp"
#include <locale>

namespace fmt
{
	//
	// Character class injection
	//

	constexpr auto
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

	template 
	<
		class Char,
		template <class> class Type = std::ctype
	>
	struct type : Type<Char>
	{
		using base = Type<Char>;
		using typename base::mask;
		using string = basic_string<Char>;
		using view = typename string::view;
		using init = typename view::init;
		using span = typename view::span;
		using pair = typename view::pair;
		using vector = typename view::vector;
		using size_type = typename view::size_type;

		static_assert(null == ~npos);

		template 
		<
			class Data
		> 
		static string from(Data const& s);

		bool check(Char c, mask x = space) const;
		// Check whether code w is an x

		auto check(view u) const;
		// Classify all characters in a view

		template <class Iterator>
		auto next(Iterator it, Iterator end, mask x = space) const
		// Next iterator after it but before end which is an x
		{
			while (it != end) 
			{
				auto const w = *it;
				if (check(w, x))
				{
					break;
				}
				else ++it;
			}
			return it;
		}

		auto next(Char const* it, Char const* end, mask x = space) const
		/// Next character after $it but before $end which is an $x
		{
			return base::scan_is(x, it, end);
		}

		auto next(view u, mask x = space) const
		/// Index of first character in view $u which is an $x
		{
			auto const from = begin(u);
			auto const to = next(from, end(u), x);
			return distance(from, to);
		}

		template <typename iterator>
		auto skip(iterator it, iterator end, mask x = space) const
		/// Next iterator after $it but before $end which is not $x
		{
			while (it != end)
			{
				auto const w = *it;
				if (check(w, x))
				{
					++it;
				}
				else break;
			}
			return it;
		}

		auto skip(Char const* it, Char const* end, mask x = space) const
		/// Next character after $it but before $end which is not $x
		{
			return base::scan_not(x, it, end);
		}

		auto skip(view u, mask x = space) const
		/// Index of first character in view $u which is not $x
		{
			auto const from = begin(u);
			auto const to = skip(from, end(u), x);
			return distance(from, to);
		}

		auto widen(fwd::basic_string_view<char> u) const
		/// Decode multibyte characters as wide type
		{
			struct iterator : utf
			{
				base const* that;
				char const* pos;
				std::size_t size;

				bool operator!=(iterator const& it) const
				{
					return it.pos != pos; 
				}

				auto operator*() const
				{
					Char c[1];
					(void) that->widen(pos, pos + size, c);
					return *c;
				}

				auto operator++()
				{
					pos += size;
					size = utf::len(pos);
					return *this;
				}

				iterator(base const* owner, char const* it)
				: that(owner), pos(it)
				{
					if (nullptr != that)
					{
						size = utf::len(pos);
					}
				}
			};

			auto const begin = u.data();
			auto const end = begin + u.size();
			return fwd::range<iterator>
			(
				{ this, begin }, { nullptr, end }
			);
		}

		auto narrow(fwd::basic_string_view<Char> u) const
		/// Encode wide characters as multibyte type
		{
			struct iterator
			{
				base const* that;
				Char const* pos;

				bool operator!=(iterator const& it) const
				{
					return it.pos != pos;
				}

				auto operator*() const
				{
					std::string s(MB_CUR_MAX, 0);
					(void) that->narrow(pos, pos + 1, 0, s.data());
					return s;
				}

				auto operator++()
				{
					++ pos;
					return *this;
				}

				iterator(base const* owner, Char const* it)
				: that(owner), pos(it)
				{ }
			};

			auto const begin = u.data();
			auto const end = begin + u.size();
			return fwd::range<iterator>
			(
				{ this, begin }, { nullptr, end }
			);
		}

		auto first(view u, mask x = space) const
		/// First iterator in view $u that is not $x
		{
			return skip(begin(u), end(u), x);
		}

		auto last(view u, mask x = space) const
		/// Last iterator in view $u that is not $x
		{
			return skip(rbegin(u), rend(u), x).base();
		}

		auto trim(view u, mask x = space) const
		/// Trim $x off the front and back of $u
		{
			auto const before = last(u, x);
			auto const after = first(u, x);
			auto const pos = std::distance(begin(u), after);
			auto const size = std::distance(after, before);
			return u.substr(pos, size);
		}

		auto divide(view u, mask x = space) const
		/// Count the quotient in $u that are $x and remainder that are not
		{
			pair n { 0, 0 };
			for (auto const w : widen(u))
			{
				if (check(w, x))
				{
					++ n.first; 
				}
				else
				{
					++ n.second;
				}
			}
			return n;
		}

		auto rem(view u, mask x = space) const
		/// Count characters in $u that are not $x
		{
			return divide(u, x).second; 
		}

		auto quot(view u, mask x = space) const
		/// Count characters in $u that are $x
		{
			return divide(u, x).first;
		}

		bool all_of(view u, mask x = space) const
		/// All decoded characters in $u are $x
		{
			return 0 == rem(u, x);
		}

		bool none_of(view u, mask x = space) const
		/// No decoded characters in $u are $x
		{
			return 0 == quot(u, x);
		}

		bool clear(view u) const
		/// All of view is white space
		{
			return all_of(u);
		}

		bool flush(view u) const
		/// None of view is white space
		{
			return none_of(u);
		}

		auto to_upper(view u) const
		/// Recode characters in upper case
		{
			string s;
			for (auto const w : widen(u))
			{
				auto const p = base::toupper(w);
				s += from(p);
			}
			return s;
		}

		auto to_lower(view u) const
		/// Recode characters in lower case
		{
			string s;
			for (auto const w : widen(u))
			{
				auto const p = base::tolower(w);
				s += from(p);
			}
			return s;
		}

		static auto to_pair(view u, view v)
		// Divide view u by first occurance of v
		{
			auto const m = u.size();
			auto const n = u.find(v);
			auto const p = u.substr(0, n);
			auto const q = u.substr(n < m ? n + 1 : m);
			return pair { p, q };
		}

		static bool terminated(view u)
		/// Check whether string is null terminated
		{
			return not empty(u) and not u[u.size()];
		}

		static auto count(view u)
		/// Count characters in view
		{
			auto n = null;
			auto m = null;
			utf mb;
			while (u.size() > m)
			{
				auto const k = mb.len(u.data() + m);
				if (k < 0)
				{
					return npos;
				}
				m += k;
				++ n;
			}
			return n;
		}

		static auto count(view u, view v)
		/// Count occurances in $u of a substring $v
		{
			auto n = null;
			auto const z = v.size();
			for (auto i = u.find(v); i != npos; i = u.find(v, i))
			{
				i += z;
				++ n;
			}
			return n;
		}

		static auto join(span t, view u)
		/// Join strings in $t with $u inserted between
		{
			string s;
			auto const z = t.size();
			for (auto i = null; i < z; ++i)
			{
				if (i) s += u;
				s += t[i];
			}
			return s;
		}

		static auto join(init n, view u)
		{
			vector v(n);
			span t(v);
			return join(t, u);
		}

		static auto split(view u, view v)
		/// Split strings in $u delimited by $v
		{
			vector t;
			auto const uz = u.size(), vz = v.size();
			for (auto i = null, j = u.find(v); i < uz; j = u.find(v, i))
			{
				auto const k = uz < j ? uz : j;
				auto const w = u.substr(i, k - i);
				if (i <= k) t.emplace_back(w);
				i = k + vz;
			}
			return t;
		}

		static auto split(span p, view v)
		{
			fwd::vector<span> t;
			auto const end = p.end();
			auto begin = p.begin();
			for (auto it = begin; it != end; ++it)
			{
				if (*it == v)
				{
					t.emplace_back(begin, it);
					begin = std::next(it);
				}
			}
			return t;
		}

		static auto replace(view u, view v, view w)
		/// Replace in u all occurrances of v with w
		{
			string s;
			auto const uz = u.size(), vz = v.size();
			for (auto i = null, j = u.find(v); i < uz; j = u.find(v, i))
			{
				auto const k = std::min(j, uz);
				s = u.substr(i, k - i);
				if (j < uz) s += w;
				i = k + vz;
			}
			return s;
		}

		static auto embrace(view u, view v)
		// Position in u with matching braces front and back in v
		{
			auto i = u.find_first_of(v.front()), j = i;
			if (i < npos)
			{
				int n = 1;
				do
				{
					j = u.find_first_of(v, j + 1);
					if (npos == j)
					{
						break; // missing
					}
					else
					if (v.back() == u[j])
					{
						--n; // close
					}
					else
					if (v.front() == u[j])
					{
						++n; // open
					}
					else
					{
						break; // other
					}
				}
				while (0 < n);
			}
			return std::pair { i, j };
		}
	};

	// Common characters

	extern type<char> const &cstr;
	extern type<wchar_t> const &wstr;

	// Multibyte shims

	template <typename iterator>
	inline auto next(iterator it, iterator end)
	{
		return cstr.next(it, end);
	}

	template <typename iterator>
	inline auto skip(iterator it, iterator end)
	{
		return cstr.skip(it, end);
	}

	inline auto widen(string::view u)
	{
		return cstr.widen(u);
	}

	inline auto narrow(wstring::view u)
	{
		return wstr.narrow(u);
	}

	inline auto first(string::view u)
	{
		return cstr.first(u);
	}

	inline auto last(string::view u)
	{
		return cstr.last(u);
	}

	inline auto trim(string::view u)
	{
		return cstr.trim(u);
	}

	inline auto to_upper(string::view u)
	{
		return cstr.to_upper(u);
	}

	inline auto to_lower(string::view u)
	{
		return cstr.to_lower(u);
	}

	inline bool terminated(string::view u)
	{
		return cstr.terminated(u);
	}

	inline auto count(string::view u)
	{
		return cstr.count(u);
	}

	inline auto count(string::view u, string::view v)
	{
		return cstr.count(u, v);
	}

	inline auto join(string::view::span t, string::view u = "")
	{
		return cstr.join(t, u);
	}

	inline auto join(string::view::init t, string::view u = "")
	{
		return cstr.join(t, u);
	}

	inline auto split(string::view u, string::view v = "")
	{
		return cstr.split(u, v);
	}

	inline auto split(string::view::span p, string::view v = "")
	{
		return cstr.split(p, v);
	}

	inline auto replace(string::view u, string::view v, string::view w)
	{
		return cstr.replace(u, v, w);
	}

	inline auto embrace(string::view u, string::view v)
	{
		return cstr.embrace(u, v);
	}

	inline auto to_pair(string::view u, string::view v = "=")
	{
		return cstr.to_pair(u, v);
	}

	inline auto to_pair(string::view::pair p, string::view u = "=")
	{
		string::view::vector x { p.first, p.second };
		return cstr.join(x, u);
	}

	inline bool same(string::view u, string::view v)
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
