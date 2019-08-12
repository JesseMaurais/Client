#ifndef fmt_hpp
#define fmt_hpp

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

	template <class Char, template <class> class Type = std::ctype> struct ctype : Type<Char>
	{
		using base = Type<Char>;
		using mask = typename base::mask;

		using string = basic_string<Char>;
		using string_pair = basic_string_pair<Char>;
		using string_range = basic_string_range<Char>;
		using string_size = basic_string_size<Char>;
		using string_size_pair = basic_string_size_pair<Char>;
		using string_vector = basic_string_vector<Char>;
		using string_vector_range = basic_string_vector_range<Char>;
		using string_span = basic_string_span<Char>;
		using string_span_range = basic_string_span_range<Char>;

		using string_view = basic_string_view<Char>;
		using string_view_pair = basic_string_view_pair<Char>;
		using string_view_range = basic_string_view_range<Char>;
		using string_view_size = basic_string_view_size<Char>;
		using string_view_size_pair = basic_string_view_size_pair<Char>;
		using string_view_vector = basic_string_view_vector<Char>;
		using string_view_vector_range = basic_string_view_vector_range<Char>;
		using string_view_span = basic_string_view_span<Char>;
		using string_view_span_range = basic_string_view_span_range<Char>;

		using size = string_size;
		using size_pair = string_size_pair;
		using iterator = typename string_view_span::iterator;

		static constexpr auto null = size {  0  };
		static constexpr auto npos = string::npos;
		
		template <typename as> static string from(as const& s);

		bool check(Char c, mask x = space) const
		/// Check whether code $w is an $x
		{
			return base::is(x, c);
		}

		auto check(string_view u) const
		/// Classify all characters in a view
		{
			std::vector<mask> x(u.size());
			base::is(u.data(), u.data() + u.size(), x.data());
			return x;
		}

		template <typename iterator>
		auto next(iterator it, iterator end, mask x = space) const
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

		auto next(string_view u, mask x = space) const
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

		auto skip(string_view u, mask x = space) const
		/// Index of first character in view $u which is not $x
		{
			auto const from = begin(u);
			auto const to = skip(from, end(u), x);
			return distance(from, to);
		}

		auto widen(basic_string_view<char> u) const
		/// Decode multibyte characters as wide type
		{
			struct iterator : utf
			{
				base const* that;
				char const* pos;
				size n;

				bool operator!=(iterator const& it) const
				{
					return it.pos != pos; 
				}

				auto operator*() const
				{
					Char c[1];
					(void) that->widen(pos, pos + n, c);
					return *c;
				}

				auto operator++()
				{
					pos += n;
					n = utf::len(pos);
					return *this;
				}

				iterator(base const* owner, char const* it)
				: that(owner), pos(it)
				{
					if (that)
					{
						n = utf::len(pos);
					}
				}
			};

			auto const begin = u.data();
			auto const end = begin + u.size();
			return range<iterator>({ this, begin }, { nullptr, end });
		}

		auto narrow(basic_string_view<Char> u) const
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

		auto first(string_view u, mask x = space) const
		/// First iterator in view $u that is not $x
		{
			return skip(begin(u), end(u), x);
		}

		auto last(string_view u, mask x = space) const
		/// Last iterator in view $u that is not $x
		{
			return skip(rbegin(u), rend(u), x).base();
		}

		auto trim(string_view u, mask x = space) const
		/// Trim $x off the front and back of $u
		{
			auto const before = last(u, x);
			auto const after = first(u, x);
			auto const pos = std::distance(begin(u), after);
			auto const size = std::distance(after, before);
			return u.substr(pos, size);
		}

		auto divide(string_view u, mask x = space) const
		/// Count the quotient in $u that are $x and remainder that are not
		{
			size_pair n { 0, 0 };
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

		auto rem(string_view u, mask x = space) const
		/// Count characters in $u that are not $x
		{
			return divide(u, x).second; 
		}

		auto quot(string_view u, mask x = space) const
		/// Count characters in $u that are $x
		{
			return divide(u, x).first;
		}

		bool all_of(string_view u, mask x = space) const
		/// All decoded characters in $u are $x
		{
			return 0 == rem(u, x);
		}

		bool none_of(string_view u, mask x = space) const
		/// No decoded characters in $u are $x
		{
			return 0 == quot(u, x);
		}

		bool clear(string_view u) const
		/// All of view is white space
		{
			return all_of(u);
		}

		bool flush(string_view u) const
		/// None of view is white space
		{
			return none_of(u);
		}

		auto to_upper(string_view u) const
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

		auto to_lower(string_view u) const
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

		static bool terminated(string_view u)
		/// Check whether string is null terminated
		{
			return not u.empty() and not u[u.size()];
		}

		static auto count(string_view u)
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

		static auto count(string_view u, string_view v)
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

		static auto join(string_view_span t, string_view u)
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

		static auto split(string_view u, string_view v)
		/// Split strings in $u delimited by $v
		{
			string_view_vector t;
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

		static auto replace(string_view u, string_view v, string_view w)
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

		static auto to_pair(string_view u, string_view v)
		/// Divide view $u by first occurance of $v
		{
			auto const m = u.size();
			auto const n = u.find(v);
			auto const p = u.substr(0, n);
			auto const q = u.substr(n < m ? n + 1 : m);
			return string_view_pair { p, q };
		}
	};

	// Common characters

	using cc = ctype<char>;
	using wc = ctype<wchar_t>;

	// C locale shims

	static thread_local cc lc;
	static thread_local wc lw;

	template <typename iterator>
	inline auto next(iterator it, iterator end)
	{
		return lc.next(it, end);
	}

	template <typename iterator>
	inline auto skip(iterator it, iterator end)
	{
		return lc.skip(it, end);
	}

	inline auto widen(string_view u)
	{
		return lc.widen(u);
	}

	inline auto narrow(wstring_view u)
	{
		return lw.narrow(u);
	}

	inline auto first(string_view u)
	{
		return lc.first(u);
	}

	inline auto last(string_view u)
	{
		return lc.last(u);
	}

	inline auto trim(string_view u)
	{
		return lc.trim(u);
	}

	inline auto to_upper(string_view u)
	{
		return lc.to_upper(u);
	}

	inline auto to_lower(string_view u)
	{
		return lc.to_lower(u);
	}

	inline bool terminated(string_view u)
	{
		return lc.terminated(u);
	}

	inline auto count(string_view u)
	{
		return lc.count(u);
	}

	inline auto count(string_view u, string_view v)
	{
		return lc.count(u, v);
	}

	inline auto join(string_view_span t, string_view u = "")
	{
		return lc.join(t, u);
	}

	inline auto split(string_view u, string_view v)
	{
		return lc.split(u, v);
	}

	inline auto replace(string_view u, string_view v, string_view w)
	{
		return lc.replace(u, v, w);
	}

	inline auto entry(string_view u)
	{
		return lc.to_pair(u, "=");
	}

	inline auto entry(string_view u, string_view v)
	{
		return lc.join({ u, v }, "=");
	}

	inline auto entry(string_view_pair p)
	{
		return entry(p.first, p.second);
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
	inline string to_string(string_view const& s)
	{
		return string(s.data(), s.size());
	}

	template <>
	inline wstring to_wstring(wstring_view const& w)
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
	inline string to_string(wstring_view const& w)
	{
		string s;
		for (auto const c : narrow(w)) s += c;
		return s;
	}

	template <>
	inline wstring to_wstring(string_view const& s)
	{
		wstring w;
		for (auto const c : widen(s)) w += c;
		return w;
	}

	template <>
	inline string to_string(wstring const& w)
	{
		return to_string(wstring_view(w));
	}

	template <>
	inline wstring to_wstring(string const& s)
	{
		return to_wstring(string_view(s));
	}

	template <>
	inline string to_string(wchar_t const*const& w)
	{
		return to_string(wstring_view(w));
	}

	template <>
	inline wstring to_wstring(char const*const& s)
	{
		return to_wstring(string_view(s));
	}

	template <>
	inline string to_string(wchar_t *const& w)
	{
		return to_string(wstring_view(w));
	}

	template <>
	inline wstring to_wstring(char *const& s)
	{
		return to_wstring(string_view(s));
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

	template <> template <class T> string cc::from(T const& s)
	{
		return ::fmt::to_string(s);
	}

	template <> template <class T> wstring wc::from(T const& s)
	{
		return ::fmt::to_wstring(s);
	}
}

#endif // file
