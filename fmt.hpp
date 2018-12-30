#ifndef fmt_hpp
#define fmt_hpp

#include <vector>
#include <codecvt>
#include <locale>
#include <string>
#include <string_view>
#include <sstream>
#include "alg.hpp"

namespace fmt
{
	// String converters for generic programming

	template <typename T>
	inline std::string to_string(T const& x)
	{
		return std::to_string(x);
	}

	template <typename T>
	inline std::wstring to_wstring(T const& x)
	{
		return std::to_wstring(x);
	}

	template <>
	inline std::string to_string(std::string const& s)
	{
		return s;
	}

	template <>
	inline std::wstring to_wstring(std::wstring const& w)
	{
		return w;
	}

	template <>
	inline std::string to_string(std::string_view const& s)
	{
		return std::string(s.data(), s.size());
	}

	template <>
	inline std::wstring to_wstring(std::wstring_view const& w)
	{
		return std::wstring(w.data(), w.size());
	}

	template <>
	inline std::string to_string(char const*const& s)
	{
		return to_string(std::string_view(s));
	}

	template <>
	inline std::wstring to_wstring(wchar_t const*const& w)
	{
		return to_wstring(std::wstring_view(w));
	}

	template <>
	inline std::string to_string(char *const& s)
	{
		return to_string(std::string_view(s));
	}

	template <>
	inline std::wstring to_wstring(wchar_t *const& w)
	{
		return to_wstring(std::wstring_view(w));
	}

	template <>
	inline std::string to_string(char const& c)
	{
		return std::string(1, c);
	}

	template <>
	inline std::wstring to_wstring(wchar_t const& c)
	{
		return std::wstring(1, c);
	}

	// Wide & narrow conversions

	template <>
	inline std::string to_string(std::wstring const& w)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.to_bytes(w);
	}

	template <>
	inline std::wstring to_wstring(std::string const& s)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.from_bytes(s);
	}

	template <>
	inline std::string to_string(std::wstring_view const& w)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.to_bytes(w.data(), w.data() + w.size());
	}

	template <>
	inline std::wstring to_wstring(std::string_view const& s)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.from_bytes(s.data(), s.data() + s.size());
	}

	template <>
	inline std::string to_string(wchar_t const*const& w)
	{
		return to_string(std::wstring_view(w));
	}

	template <>
	inline std::wstring to_wstring(char const*const& s)
	{
		return to_wstring(std::string_view(s));
	}

	template <>
	inline std::string to_string(wchar_t *const& w)
	{
		return to_string(std::wstring_view(w));
	}

	template <>
	inline std::wstring to_wstring(char *const& s)
	{
		return to_wstring(std::string_view(s));
	}

	template <>
	inline std::string to_string(wchar_t const& c)
	{
		return to_string(std::wstring(1, c));
	}

	template <>
	inline std::wstring to_wstring(char const& c)
	{
		return to_wstring(std::string(1, c));
	}

	inline bool empty(std::string const& s)
	{
		return s.empty();
	}

	inline bool empty(std::wstring const& s)
	{
		return s.empty();
	}

	inline bool empty(std::string_view s)
	{
		return s.empty();
	}

	inline bool empty(std::wstring_view s)
	{
		return s.empty();
	}

	// Aggregate types

	using pair = std::pair<std::string, std::string>;
	using span_view = std::vector<std::string_view>;

	inline bool empty(pair const &p)
	{
		return empty(p.first) and empty(p.second);
	}

	inline bool empty(span_view const &v)
	{
		return v.empty() or stl::all_of(v, [](auto u) { return empty(u); });
	}

	// Basic string formatting tools

	struct ctype : std::ctype<char> { };
	struct wctype : std::ctype<wchar_t> { };
	enum char_type : std::ctype_base::mask
	{
		space = std::ctype_base::space,
		print = std::ctype_base::print,
		cntrl = std::ctype_base::cntrl,
		upper = std::ctype_base::upper,
		lower = std::ctype_base::lower,
		alpha = std::ctype_base::alpha,
		digit = std::ctype_base::digit,
		punct = std::ctype_base::punct,
		blank = std::ctype_base::blank,
		graph = std::ctype_base::graph,
	};

	inline std::string to_upper(std::string_view u)
	{
		wctype cc;
		std::wstring w = to_wstring(u);
		for (wchar_t &c : w) c = cc.toupper(c);
		return to_string(w);
	}

	inline std::string to_lower(std::string_view u)
	{
		wctype cc;
		std::wstring w = to_wstring(u);
		for (wchar_t &c : w) c = cc.tolower(c);
		return to_string(w);
	}

	template <typename Iterator>
	Iterator skip(Iterator begin, Iterator end, char_type type = space)
	{
		ctype cc;
		Iterator it;
		for (it = begin; it != end and cc.is(type, *it); ++it);
		return it;
	}

	inline std::string::iterator trim_begin(std::string &s)
	{
		auto it = skip(begin(s), end(s));
		return s.erase(begin(s), it);
	}

	inline std::string::iterator trim_end(std::string &s)
	{
		auto it = skip(rbegin(s), rend(s));
		return s.erase(it.base(), end(s));
	}

	inline bool trim(std::string &s)
	{
		return trim_begin(s) != trim_end(s);
	}

	inline std::string join(span_view const& tok, std::string const& del)
	{
		std::stringstream ss;
		auto const n = tok.size();
		for (decltype(n) i = 0; i < n; ++i)
		{
			if (i) ss << del;
			ss << t[i];
		}
		return ss.str();
	}

	inline span_view split(std::string_view u, std::string_view v)
	{
		span_view tok;
		auto const uz = u.size(), vz = v.size();
		for (decltype(z) i = 0, j = u.find(v); i < uz; j = u.find(v, i))
		{
			auto n = j < z ? j - i : z - i;
			tok.emplace_back(u.substr(i, n));
			i += n + vz;
		}
		return tok;
	}

	inline void replace(std::string& s, std::string_view u, std::string_view v)
	{
		s = join(split(s, u), v);
	}

	// String formatting with inline tags

	class format
	{
	public:

		format(std::string_view s, std::string_view begin_tag="{", std::string_view end_tag="}")
			: buffer(to_string(s))
			, begin(begin_tag)
			, end(end_tag)
			, index(0)
		{}

		template <typename T> format& operator % (T&& arg)
		{
			replace(buffer, next_tag(), to_string(arg));
			return *this;
		}

		operator std::string_view() const
		{
			return buffer;
		}

		operator std::string()
		{
			return buffer;
		}

	private:

		std::string buffer, begin, end;
		std::string::size_type index;

		std::string next_tag()
		{
			std::stringstream tag;
			tag << begin << ++index << end;
			return tag.str();
		}
	};

	inline std::string quote(std::string_view u)
	{
		return format("\"{1}\"") % u;
	}

	inline std::string key_value(std::string_view u, std::string_view v)
	{
		return format("{1}={2}") % u % v;
	}

	inline pair key_value(std::string_view u)
	{
		auto const n = u.find('=');
		auto const key = u.substr(0, n);
		auto const value = u.substr(n + 1);
		return pair(key, value);
	}
}

#endif // file