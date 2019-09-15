#ifndef ops_hpp
#define ops_hpp

#include <cstddef>
#include <cstdint>
#include "str.hpp"

namespace sys
{
	using size_t = std::size_t;
	using ssize_t = std::ptrdiff_t;

	constexpr int invalid = -1;

	inline bool fail(int value)
	{
		return invalid == value;
	}

	struct ops
	{
		virtual ssize_t read(void *buf, size_t sz) const = 0;
		virtual ssize_t write(const void *buf, size_t sz) const = 0;

		template <typename C>
		ssize_t read(C *buf, size_t sz) const
		{
			return read(static_cast<void*>(buf), sz);
		}

		template <typename C>
		ssize_t write(const C *buf, size_t sz) const
		{
			return write(static_cast<const void*>(buf), sz);
		}

		template <typename C>
		ssize_t read(C &buf) const
		{
			return read(&buf, sizeof buf);
		}

		template <typename C>
		ssize_t write(const C &buf) const
		{
			return write(&buf, sizeof buf);
		}

		ssize_t read(fmt::string& s) const
		{
			return read(data(s), size(s));
		}

		ssize_t write(fmt::string_view u) const
		{
			return write(data(u), size(u));
		}

		ssize_t read(fmt::wstring& w) const
		{
			constexpr auto sz = sizeof (wchar_t);
			return read(data(w), size(w) * sz) / sz;
		}

		ssize_t write(fmt::wstring_view w) const
		{
			constexpr auto sz = sizeof (wchar_t);
			return write(data(w), size(w) * sz) / sz;
		}
	};
}

#endif // file

