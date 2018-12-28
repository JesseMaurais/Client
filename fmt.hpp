#ifndef fmt_hpp
#define fmt_hpp

#include <locale>
#include <codecvt>
#include <string>
#include <string_view>
#include <sstream>
#include "alg.hpp"

namespace fmt
{
	// String converters for generic programming

	template <typename T>
	inline std::string to_string(T const &x)
	{
		return std::to_string(x);
	}

	template <typename T>
	inline std::wstring to_wstring(T const &x)
	{
		return std::to_wstring(x);
	}

	template <>
	inline std::string to_string(std::string const &s)
	{
		return s;
	}

	template <>
	inline std::wstring to_string(std::wstring const &w)
	{
		return w;
	}

	template <>
	inline std::string to_string(std::string_view const &s)
	{
		return std::string(s.data(), s.size());
	}

	template <>
	inline std::wstring to_wstring(std::wstring_view const &w)
	{
		return std::wstring(w.data(), w.size());
	}

	template <>
	inline std::string to_string(char const * const &s)
	{
		return to_string(std::string_view(s));
	}

	template <>
	inline std::wstring to_wstring(wchar_t const * const &w)
	{
		return to_wstring(std::wstring_view(w));
	}

	template <>
	inline std::string to_string(char * const &s)
	{
		return to_string(std::string_view(s));
	}

	template <>
	inline std::wstring to_wstring(wchar_t * const &w)
	{
		return to_wstring(std::wstring_view(w));
	}

	template <>
	inline std::string to_string(char const &c)
	{
		return std::string(1, c);
	}

	template <>
	inline std::wstring to_wstring(wchar_t const &c)
	{
		return std::wstring(1, c);
	}

	// Wide & narrow conversions

	template <>
	inline std::string to_string(std::wstring const &w)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.to_bytes(w);
	}

	template <>
	inline std::wstring to_wstring(std::string const &s)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.from_bytes(s);
	}

	template <>
	inline std::string to_string(std::wstring_view const &w)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.to_bytes(w.data(), w.data() + w.size());
	}

	template <>
	inline std::wstring to_wstring(std::string_view const &s)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.from_bytes(s.data(), s.data() + s.size();
	}

	template <>
	inline std::string to_string(wchar_t const * const &w)
	{
		return to_string(std::wstring_view(w));
	}

	template <>
	inline std::wstring to_wstring(char const * const &s)
	{
		return to_wstring(std::string_view(s));
	}

	template <>
	inline std::string to_string(wchar_t * const &w)
	{
		return to_string(std::wstring_view(w));
	}

	template <>
	inline std::wstring to_wstring(char * const &s)
	{
		return to_wstring(std::string_view(s));
	}

	template <>
	inline std::string to_string(wchar_t const &c)
	{
		return to_string(std::wstring(1, c));
	}

	template <>
	inline std::wstring to_wstring(char const &c)
	{
		return to_wstring(std::string(1, c));
	}

	inline bool empty(std::string const &s)
	{
		return s.empty();
	}

	inline bool empty(std::wstring const &s)
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
	using map = std::map<std::string, std::string>;
	using view = std::vector<std::string_view>;

	inline bool empty(pair const &p)
	{
		return empty(p.first) and empty(p.second);
	}

	inline bool empty(map const &m)
	{
		return m.empty() or stl::all_of(m, empty);
	}

	inline bool empty(view const &s)
	{
		return s.empty() or stl::all_of(s, empty);
	}

	// Basic string formatting tools

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
		std::ctype<wchar_t> cc;
		std::wstring w = to_wstring(u);
		for (wchar_t &c : w) c = cc.toupper(c);
		return to_string(w);
	}

	inline std::string to_lower(std::string_view u)
	{
		std::ctype<wchar_t> cc;
		std::wstring w = to_wstring(u);
		for (wchar_t &c : w) c = cc.tolower(c);
		return to_string(w);
	}

	template <typename Iterator>
	Iterator skip(char_type type, Iterator begin, Iterator end)
	{
		Iterator it;
		std::ctype<char> cc;
		for (it = begin; it != end; ++it) if (not cc.is(type, *it)) break;
		return it;
	}

	inline std::string::iterator trim_begin(std::string &s)
	{
		auto it = skip(space, s.begin(), s.end());
		return s.erase(s.begin(), it);
	}

	inline std::string::iterator trim_end(std::string &s)
	{
		auto it = skip(space, s.rbegin(), s.rend());
		return s.erase(std::next(it).base(), s.end());
	}

	inline bool trim(std::string &s)
	{
		return trim_begin(s) != trim_end(s);
	}

	static std::istream& getline(std::istream& in, std::string& s)
	{
		while (std::getline(in, s))
		{
			constexpr auto c = '#';

			auto const it = skip(space, s.begin(), s.end());
			if (it != s.end() and c != *it)
			{
				auto const t = s.find(c);
				s = s.substr(0, t);
				if (trim(s))
				{
					break;
				}
			}
		}
		return is;
	}

	inline view split(std::string_view u, std::string_view w)
	{
		view tok;
		using size_type = std::string_view::size_type;
		constexpr size_type n = std::string_view::npos;
		for (size_type i = 0, j = u.find_first_of(w); i != n; j = u.find_first_of(w, i))
		{
			if (i != j)
			{
				auto t = u.substr(i, j - i);
				tok.emplace_back(t);
			}
			i = u.find_first_not_of(w, j);
		}
		return tok;
	}

	inline std::string join(view const& tok, std::string const& del)
	{
		std::stringstream stream;
		auto it = std::ostream_iterator<std::string>(stream, del.c_str());
		std::copy(std::begin(tok), std::end(tok), it);
		return stream.str();
	}

	inline void replace(std::string &buf, std::string_view u, std::string_view v)
	{
		using size_type = std::string_view::size_type;
		constexpr size_type n = std::string::npos;
		for (auto i = buf.find(u.data(), 0, u.size()); i != n; i = buf.find(u.data(), i + v.size(), u.size()))
		{
			buf.replace(i, u.size(), v.data(), v.size());
		}
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
		auto const part = split(u, "=");
		auto const key = part.front();
		auto const value = part.back();
		return pair(key, value);
	}
}

#endif // file