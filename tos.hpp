#ifndef tos_hpp
#define tos_hpp

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

	//
	// Wide & narrow conversions
	//

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
}

#endif // file
