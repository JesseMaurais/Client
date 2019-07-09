#ifndef fmt_hpp
#define fmt_hpp

#include <algorithm>
#include <functional>
#include <cinttypes>
#include <cstdlib>
#include <locale>
#include "str.hpp"
#include "its.hpp"
#include "tos.hpp"

namespace fmt
{
	//
	// Character class injection
	//

	template <class Char> struct ctype : std::ctype<Char>
	{
		using base = std::ctype<Char>;
		using mask = typename base::mask;

		using string = fmt::basic_string<Char>;
		using string_pair = fmt::basic_string_pair<Char>;
		using string_range = fmt::basic_string_range<Char>;
		using string_size = fmt::basic_string_size<Char>;
		using string_size_pair = fmt::basic_string_size_pair<Char>;
		using string_vector = fmt::basic_string_vector<Char>;
		using string_vector_range = fmt::basic_string_vector_range<Char>;
		using string_span = fmt::basic_string_span<Char>;
		using string_span_range = fmt::basic_string_span_range<Char>;

		using string_view = fmt::basic_string_view<Char>;
		using string_view_pair = fmt::basic_string_view_pair<Char>;
		using string_view_range = fmt::basic_string_view_range<Char>;
		using string_view_size = fmt::basic_string_view_size<Char>;
		using string_view_size_pair = fmt::basic_string_view_size_pair<Char>;
		using string_view_vector = fmt::basic_string_view_vector<Char>;
		using string_view_vector_range = fmt::basic_string_view_vector_range<Char>;
		using string_view_span = fmt::basic_string_view_span<Char>;
		using string_view_span_range = fmt::basic_string_view_span_range<Char>;

		using iterator = typename string_view_span::iterator;
		using size_pair = string_size_pair;
		using size = string_size;
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
			std::vector<mask> x(u.size());
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

		using reactor = std::function<void(iterator, size)>;
		static void tag(iterator begin, iterator end, string_view u, reactor f)
		/// Call $f for any in sorted range $[begin, end) that is found in $u
		{
			auto it = begin;
			auto const z = u.size();
			for (size i = 0, j = 1; j <= z; ++j)
			{
				auto const n = j - i;
				auto const v = u.substr(i, n);

				it = std::lower_bound(it, end, v);
				if (end == it)
				{
					i = j;
					it = begin;
					continue;
				}

				if (it->size() == n)
				{
					f(it, i);
				}

				it = std::upper_bound(it, end, v);
				if (end == it or it->compare(0, n, v))
				{
					i = j;
					it = begin;
				}
			}
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

	inline void tag(string_view_span t, string_view u, cc::reactor r)
	{
		return lc.tag(begin(t), end(t), u, r);
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
