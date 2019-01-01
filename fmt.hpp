#ifndef fmt_hpp
#define fmt_hpp

#include <codecvt>
#include <locale>
#include <sstream>
#include "str.hpp"
#include "it.hpp"
#include "alg.hpp"

namespace fmt
{
	// String converters for generic programming

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
	inline string to_string(wstring const& w)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.to_bytes(w);
	}

	template <>
	inline wstring to_wstring(string const& s)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.from_bytes(s);
	}

	template <>
	inline string to_string(wstring_view const& w)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.to_bytes(w.data(), w.data() + w.size());
	}

	template <>
	inline wstring to_wstring(string_view const& s)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.from_bytes(s.data(), s.data() + s.size());
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
		return s.data()[s.size()] == '\0';
	}

	// Basic string formatting tools

	struct ctype : std::ctype<char> { };
	struct wctype : std::ctype<wchar_t> { };
	enum char_mask : std::ctype_base::mask
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

	inline string to_upper(string_view u)
	{
		wctype cc;
		string s;
		for (wchar_t w : widen(u)) s += to_string(cc.toupper(w));
		s.shrink_to_fit();
		return s;
	}

	inline string to_lower(string_view u)
	{
		wctype cc;
		string s;
		for (wchar_t w : widen(u)) s += to_string(cc.tolower(w));
		s.shrink_to_fit();
		return s;
	}

	template <typename Iterator>
	Iterator skip(Iterator begin, Iterator end, char_mask type = space)
	{
		ctype cc;
		Iterator it;
		for (it = begin; it != end and cc.is(type, *it); ++it);
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

	inline string join(span_view const& tok, string const& del)
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
			size const n = std::min(uz, j) - i;
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
			size const n = std::min(uz, j) - i;
			ss << u.substr(i, n);
			if (j < uz) ss << w;
			i += n + vz;
		}
		return ss.str();
	}

	// String formatting with inline tags

	class format
	{
	public:

		format(string_view s, string_view begin_tag = "{", string_view end_tag = "}")
			: buffer(to_string(s))
			, begin(begin_tag)
			, end(end_tag)
			, index(0)
		{}

		template <typename T> format& operator % (T&& arg)
		{
			buffer = replace(buffer, next_tag(), to_string(arg));
			return *this;
		}

		operator string_view() const
		{
			return buffer;
		}

		operator string()
		{
			return buffer;
		}

	private:

		string buffer, begin, end;
		string::size_type index;

		string next_tag()
		{
			std::stringstream tag;
			tag << begin << ++index << end;
			return tag.str();
		}
	};

	inline string quote(string_view u)
	{
		return format("\"{1}\"") % u;
	}

	inline string key_value(string_view u, string_view v)
	{
		return format("{1}={2}") % u % v;
	}

	inline pair key_value(string_view u)
	{
		auto n = u.find('=');
		auto key = u.substr(0, n);
		auto value = u.substr(n + 1);
		return pair(key, value);
	}
}

#endif // file
