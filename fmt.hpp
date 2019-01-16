#ifndef fmt_hpp
#define fmt_hpp

#include <locale>
#include <sstream>
#include "str.hpp"
#include "wcs.hpp"
#include "alg.hpp"

namespace fmt
{
	// Generic string converters

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
		return to_string(string_view(s));
	}

	template <>
	inline wstring to_wstring(wchar_t const*const& w)
	{
		return to_wstring(wstring_view(w));
	}

	template <>
	inline string to_string(char *const& s)
	{
		return to_string(string_view(s));
	}

	template <>
	inline wstring to_wstring(wchar_t *const& w)
	{
		return to_wstring(wstring_view(w));
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
		for (char const c : narrow(w)) s.push_back(c);
		return s;
	}

	template <>
	inline wstring to_wstring(string_view const& s)
	{
		wstring w;
		for (wchar_t const c : widen(s)) w.push_back(c);
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

	inline bool empty(pair const &p)
	{
		return empty(p.first) and empty(p.second);
	}

	inline bool empty(span_view const &v)
	{
		return v.empty() or stl::all_of(v, [](auto u) { return empty(u); });
	}

	inline bool terminated(string_view s)
	{
		return not empty(s) and s.data()[s.size()] == '\0';
	}

	// Basic string formatting tools

	struct char_type : std::ctype<char> { };
	struct wchar_type : std::ctype<wchar_t> { };

	using char_mask = std::ctype_base::mask;

	inline string to_upper(string_view u)
	{
		string s;
		wchar_type cc;
		for (wchar_t w : widen(u)) s += to_string(cc.toupper(w));
		s.shrink_to_fit();
		return s;
	}

	inline string to_lower(string_view u)
	{
		string s;
		wchar_type cc;
		for (wchar_t w : widen(u)) s += to_string(cc.tolower(w));
		s.shrink_to_fit();
		return s;
	}

	template <typename Iterator>
	Iterator skip(Iterator begin, Iterator end, char_mask mask = char_type::space)
	{
		char_type cc;
		Iterator it;
		for (it = begin; it != end and cc.is(mask, *it); ++it);
		return it;
	}

	inline string::iterator trim_begin(string &s)
	{
		auto it = skip(begin(s), end(s));
		return s.erase(begin(s), it);
	}

	inline string::iterator trim_end(string &s)
	{
		auto it = skip(rbegin(s), rend(s));
		return s.erase(it.base(), end(s));
	}

	inline bool trim(string &s)
	{
		return trim_begin(s) != trim_end(s);
	}

	inline string join(span_view const& tok, string const& del = "")
	{
		std::stringstream ss;
		using size = span_view::size_type;
		size const z = tok.size();
		for (size i = 0; i < z; ++i)
		{
			if (i) ss << del;
			ss << tok[i];
		}
		return ss.str();
	}

	inline span_view split(string_view u, string_view v)
	{
		span_view tok;
		using size = string_view::size_type;
		size const uz = u.size(), vz = v.size();
		for (size i = 0, j = u.find(v); i < uz; j = u.find(v, i))
		{
			size const n = (uz < j ? uz : j) - i;
			if (n) tok.emplace_back(u.substr(i, n));
			i += n + vz;
		}
		return tok;
	}

	inline string replace(string_view u, string_view v, string_view w)
	{
		std::stringstream ss;
		using size = string_view::size_type;
		size const uz = u.size(), vz = v.size();
		for (size i = 0, j = u.find(v); i < uz; j = u.find(v, i))
		{
			size const n = (uz < j ? uz : j) - i;
			ss << u.substr(i, n);
			if (j < uz) ss << w;
			i += n + vz;
		}
		return ss.str();
	}

	// Common parsing of key/value pairs

	inline string key_value(string_view u, string_view v)
	{
		return join({u, v}, "=");
	}

	inline pair key_value(string_view u)
	{
		auto const m = u.size();
		auto const n = u.find('=');
		auto const key = u.substr(0, n);
		auto const value = u.substr(n < m ? n + 1 : m);
		return pair(key, value);
	}
}

#endif // file
