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

	template <>
	inline std::string to_string(std::string const &s)
	{
		return s;
	}

	template <>
	inline std::string to_string(std::string_view const &s)
	{
		return std::string(s.data(), s.size());
	}

	template <>
	inline std::string to_string(char const * const &s)
	{
		return to_string(std::string_view(s));
	}

	template <>
	inline std::string to_string(char * const &s)
	{
		return to_string(std::string_view(s));
	}

	template <>
	inline std::string to_string(char const &c)
	{
		return std::string(1, c);
	}

	template <>
	inline std::string to_string(std::wstring const &w)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.to_bytes(w);
	}

	template <>
	inline std::string to_string(std::wstring_view const &w)
	{
		using utf8 = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<utf8> convert;
		return convert.to_bytes(w.data(), w.data() + w.size());
	}

	template <>
	inline std::string to_string(wchar_t const * const &s)
	{
		return to_string(std::wstring_view(s));
	}

	template <>
	inline std::string to_string(wchar_t * const &s)
	{
		return to_string(std::wstring_view(s));
	}

	template <>
	inline std::string to_string(wchar_t const &c)
	{
		return to_string(std::wstring(1, c));
	}

	using pair = std::pair<std::string, std::string>;
	using map = std::map<std::string, std::string>;
	using span = std::vector<std::string_view>;

	inline bool empty(std::string const &s)
	{
		return s.empty();
	}

	inline bool empty(std::string_view s)
	{
		return s.empty();
	}

	inline bool empty(pair const &p)
	{
		return empty(p.first) and empty(p.second);
	}

	inline bool empty(span const &s)
	{
		return s.empty() or stl::all_of(s, empty);
	}

	// Basic string formatting tools

	inline void replace(std::string &buf, std::string_view u, std::string_view v)
	{
		using size_type = std::string_view::size_type;
		constexpr size_type n = std::string::npos;
		for (auto i = buf.find(s.data(), 0, u.size()); i != n; i = buf.find(u.data(), i + v.size(), u.size()))
		{
			buf.replace(i, u.size(), v.data(), v.size());
		}
	}

	inline span split(std::string_view u, std::string_view w)
	{
		span tok;
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

	inline std::string join(span const& tok, std::string const& del)
	{
		std::stringstream stream;
		auto it = std::ostream_iterator<std::string>(stream, del.c_str());
		std::copy(std::begin(tok), std::end(tok), it);
		return stream.str();
	}

	inline std::string to_upper(std::string s)
	{
		for (char &c : s) c = std::toupper(c);
		return s;
	}

	inline std::string to_lower(std::string s)
	{
		for (char &c : s) c = std::tolower(c);
		return s;
	}

	inline std::string::iterator trim_begin(std::string &s)
	{
		auto it = s.begin();
		while (it != s.end() and std::isblank(*it)) ++it;
		return s.erase(s.begin(), it);
	}

	inline std::string::iterator trim_end(std::string &s)
	{
		auto it = s.rbegin();
		while (it != s.rend() and std::isblank(*it)) ++it;
		return s.erase(s.rbegin(), it);
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
			constexpr auto ws = " \t";

			auto const n = s.find_first_not_of(ws);
			if (npos != n and c != s[n])
			{
				auto const t = s.find(c);
				s = s.substr(0, t);
				trim(s);
				break;
			}
		}
		return is;
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