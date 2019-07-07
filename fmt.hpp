#ifndef fmt_hpp
#define fmt_hpp

#include <cinttypes>
#include <cstdlib>
#include <locale>
#include "str.hpp"
#include "its.hpp"

namespace fmt
{
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

	// Wide & narrow conversions

	template <>
	inline string to_string(wstring_view const& w)
	{
		string s;
		for (auto const c : narrow(w)) s.push_back(c);
		return s;
	}

	template <>
	inline wstring to_wstring(string_view const& s)
	{
		wstring w;
		for (auto const c : widen(s)) w.push_back(c);
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

	// Basic predicates

	inline bool empty(string const& s)
	{
		return s.empty();
	}

	inline bool empty(wstring const& s)
	{
		return s.empty();
	}

	inline bool empty(string_view s)
	{
		return s.empty();
	}

	inline bool empty(wstring_view s)
	{
		return s.empty();
	}

	inline bool empty(string_pair const& p)
	{
		return empty(p.first) and empty(p.second);
	}

	inline bool empty(string_view_pair const& p)
	{
		return empty(p.first) and empty(p.second);
	}

	inline bool empty(span_view const &v)
	{
		for (auto const& u : v)
		{
			if (not empty(u))
			{
				return false;
			}
		}
		return true;
	}

	//
	// Character class injection
	//

	template <class Char> struct ctype : std::ctype<Char>
	{
		using base = std::ctype<Char>;
		using mask = typename base::mask;
		using string_mask = std::vector<mask>;	
		using string = fmt::basic_string<Char>;
		using string_view = fmt::basic_string_view<Char>;
		using string_view_pair = fmt::basic_string_view_pair<Char>;
		using string_view_vector = fmt::basic_string_view_vector<Char>;
		using string_vector = fmt::basic_string_vector<Char>;
		using string_pair = fmt::basic_string_pair<Char>;
		using size_pair = fmt::basic_string_size_pair<Char>;
		using size = typename string::size_type;
		using div = std::imaxdiv_t;

		static_assert(sizeof (div) == sizeof (size_pair), "Conformance failure.");

		static constexpr auto null = size { 0 };
		static constexpr auto npos = string::npos;
		static constexpr auto space = base::space;

		template <typename as> static string from(as const& s);

		bool check(wint_t w, mask x = space) const
		/// Check whether code $w is an $x
		{
			return base::is(x, w);
		}

		auto type(string_view u) const
		/// Classify characters in view $u
		{
			string_mask x(u.size());
			base::is(u.data(), u.data() + u.size(), x.data());
			return x;
		}

		mask type(wint_t w) const
		/// Classify character $w
		{
			auto const x = type(string_view(w));
			return empty(x) ? 0 : x.front();
		}

		template <typename iterator>
		auto next(iterator it, iterator end, mask x = space) const
		/// Next iterator after $it but not after $end which is an $x
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

		auto first(string_view u, mask x = space) const
		/// First iterator in view $u that is not $x
		{
			return next(begin(u), end(u), x);
		}

		auto last(string_view u, mask x = space) const
		/// Last iterator in view $u that is not $x
		{
			return next(rbegin(u), rend(u), x).base();
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
			div n { 0, 0 };
			for (auto const w : widen(u))
			{
				if (check(w, x))
				{
					++ n.quot; 
				}
				else
				{
					++ n.rem;
				}
			}
			return n;
		}

		auto rem(string_view u, mask x = space) const
		/// Count characters in $u that are not $x
		{
			return divide(u, x).rem; 
		}

		auto quot(string_view u, mask x = space) const
		/// Count characters in $u that are $x
		{
			return divide(u, x).quot;
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
			for (auto const w : widen(u))
			{
				(void) w;
				++n;
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

		static auto join(span_view t, string_view u)
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

	template <> template <class T> string cc::from(T const& s)
	{
		return ::fmt::to_string(s);
	}

	template <> template <class T> wstring wc::from(T const& s)
	{
		return ::fmt::to_wstring(s);
	}

	// C locale shims

	static thread_local cc lc;

	template <typename iterator>
	inline auto next(iterator it, iterator end)
	{
		return lc.next(it, end);
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

	inline auto terminated(string_view u)
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

	inline auto join(span_view t, string_view u = "")
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

	inline auto to_pair(string_view u, string_view v = "=")
	{
		return lc.to_pair(u, v);
	}

	inline auto key_value(string_view u, string_view v)
	{
		return join({ u, v }, "=");
	}
}

#endif // file
