#ifndef form_hpp
#define form_hpp

#include <cstddef>
#include <cstdint>
#include "str.hpp"

namespace env::file
{
	using size_t = std::size_t;
	using ssize_t = std::ptrdiff_t;

	struct form
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
			return read(s.data(), s.size());
		}

		ssize_t write(fmt::view u) const
		{
			return write(u.data(), u.size());
		}

		ssize_t read(fmt::wstring& w) const
		{
			constexpr auto sz = sizeof (wchar_t);
			return read(w.data(), w.size() * sz) / sz;
		}

		ssize_t write(fmt::wview w) const
		{
			constexpr auto sz = sizeof (wchar_t);
			return write(w.data(), w.size() * sz) / sz;
		}
	};
}

#endif // file
