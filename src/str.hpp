#ifndef str_hpp
#define str_hpp

#include <locale>
#include "str.hpp"
#include "utf.hpp"

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
		using traits = memory_traits<type>;
		using string = basic_string<Char>;

		template 
		<
			class Type
		> 
		static string from(Type const& s);

		bool check(Char c, mask x = space) const
		/// Check whether code $w is an $x
		{
			return base::is(x, c);
		}

		auto check(string::view u) const
		/// Classify all characters in a view
		{
			vector<mask> x(u.size());
			base::is(u.data(), u.data() + u.size(), x.data());
			return x;
		}

		template <class Iterator>
		auto next(Iterator it, Iterator end, mask x = space) const
		/// Next iterator after $it but before $end which is an $x
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

		auto next(string::view u, mask x = space) const
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

		auto skip(string::view u, mask x = space) const
		/// Index of first character in view $u which is not $x
		{
			auto const from = begin(u);
			auto const to = skip(from, end(u), x);
			return distance(from, to);
		}

		auto widen(basic_string<char>::view u) const
		/// Decode multibyte characters as wide type
		{
			struct iterator : utf
			{
				base const* that;
				char const* pos;
				size_type size;

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
			return range({ this, begin }, { nullptr, end });
		}

		auto narrow(basic_string::view<Char> u) const
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
			return range<iterator>({ this, begin }, { nullptr, end });
		}

		auto first(string::view u, mask x = space) const
		/// First iterator in view $u that is not $x
		{
			return skip(begin(u), end(u), x);
		}

		auto last(string::view u, mask x = space) const
		/// Last iterator in view $u that is not $x
		{
			return skip(rbegin(u), rend(u), x).base();
		}

		auto trim(string::view u, mask x = space) const
		/// Trim $x off the front and back of $u
		{
			auto const before = last(u, x);
			auto const after = first(u, x);
			auto const pos = std::distance(begin(u), after);
			auto const size = std::distance(after, before);
			return u.substr(pos, size);
		}

		auto divide(string::view u, mask x = space) const
		/// Count the quotient in $u that are $x and remainder that are not
		{
			pair<size_t> n { 0, 0 };
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

		auto rem(string::view u, mask x = space) const
		/// Count characters in $u that are not $x
		{
			return divide(u, x).second; 
		}

		auto quot(string::view u, mask x = space) const
		/// Count characters in $u that are $x
		{
			return divide(u, x).first;
		}

		bool all_of(string::view u, mask x = space) const
		/// All decoded characters in $u are $x
		{
			return 0 == rem(u, x);
		}

		bool none_of(string::view u, mask x = space) const
		/// No decoded characters in $u are $x
		{
			return 0 == quot(u, x);
		}

		bool clear(string::view u) const
		/// All of view is white space
		{
			return all_of(u);
		}

		bool flush(string::view u) const
		/// None of view is white space
		{
			return none_of(u);
		}

		auto to_upper(string::view u) const
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

		auto to_lower(string::view u) const
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

		static bool terminated(string::view u)
		/// Check whether string is null terminated
		{
			return not empty(u) and not u[u.size()];
		}

		static auto count(string::view u)
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

		static auto count(string::view u, string::view v)
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

		static auto join(string::view_span t, string::view u)
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

		static auto join(string::view::list t, string::view u)
		{
			string::view::vector v(t.begin(), t.end());
			return join(string::view::span(v), u);
		}

		static auto split(string::view u, string::view v)
		/// Split strings in $u delimited by $v
		{
			string::view::vector t;
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

		static auto replace(string::view u, string::view v, string::view w)
		/// Replace in $u all occurrances of $v with $w
		{
			string s;
			auto const uz = u.size(), vz = v.size();
			for (auto i = null, j = u.find(v); i < uz; j = u.find(v, i))
			{
				auto const k = uz < j ? uz : j;
				s += u.substr(i, k - i);
				if (j < uz) s += w;
				i = k + vz;
			}
			return s;
		}

		static auto embrace(string::view u, string::view v)
		// First matching braces $v front and $v back found in $u
		{
			auto i = u.find_first_of(v.front()), j = i;
			if (i < npos)
			{
				size_type n = 1;
				do
				{
					j = u.find_first_of(v, j + 1);
					if (npos == j)
					{
						break;
					}
					else
					if (u[j] == v.back())
					{
						-- n;
					}
					else
					if (u[j] == v.front())
					{
						++ n;
					}
					else // $v interior
					{
						break;
					}
				}
				while (0 < n);
			}
			return std::pair{ i, j };
		}

		static auto to_pair(string::view u, string::view v)
		/// Divide view $u by first occurance of $v
		{
			auto const m = u.size();
			auto const n = u.find(v);
			auto const p = u.substr(0, n);
			auto const q = u.substr(n < m ? n + 1 : m);
			return make_pair(p, q);
		}
	};

	// Common characters

	extern type<char>::ref str;
	extern type<wchar_t>::ref wstr;

	// C locale shims

	template <typename iterator>
	inline auto next(iterator it, iterator end)
	{
		return str.next(it, end);
	}

	template <typename iterator>
	inline auto skip(iterator it, iterator end)
	{
		return str.skip(it, end);
	}

	inline auto widen(string::view u)
	{
		return str.widen(u);
	}

	inline auto narrow(wstring::view u)
	{
		return wstr.narrow(u);
	}

	inline auto first(string::view u)
	{
		return str.first(u);
	}

	inline auto last(string::view u)
	{
		return str.last(u);
	}

	inline auto trim(string::view u)
	{
		return str.trim(u);
	}

	inline auto to_upper(string::view u)
	{
		return str.to_upper(u);
	}

	inline auto to_lower(string::view u)
	{
		return str.to_lower(u);
	}

	inline bool terminated(string::view u)
	{
		return str.terminated(u);
	}

	inline auto count(string::view u)
	{
		return str.count(u);
	}

	inline auto count(string::view u, string::view v)
	{
		return str.count(u, v);
	}

	inline auto join(string::view::span t, string::view u = "")
	{
		return str.join(t, u);
	}

	inline auto join(string::view::list t, string::view u = "")
	{
		return str.join(t, u);
	}

	inline auto split(string::view u, string::view v = "")
	{
		return str.split(u, v);
	}

	inline auto replace(string::view u, string::view v, string::view w)
	{
		return str.replace(u, v, w);
	}

	inline auto embrace(string::view u, string::view v)
	{
		return str.embrace(u, v);
	}

	inline auto entry(string::view u)
	{
		return str.to_pair(u, "=");
	}

	inline auto entry(string::view u, string::view v)
	{
		string::view::vector w { u, v };
		return str.join(w, "=");
	}

	inline auto entry(string::view::pair p)
	{
		return entry(p.first, p.second);
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
