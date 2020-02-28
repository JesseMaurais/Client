#ifndef file_hpp
#define file_hpp

#include <cstddef>
#include <cstdint>
#include "str.hpp"

namespace env::file
{
	using size_t = std::size_t;
	using ssize_t = std::ptrdiff_t;

	struct reader
	{
		virtual ssize_t read(void *buf, size_t sz) const = 0;

		template <typename C>
		ssize_t read(C *buf, size_t sz = sizeof(C)) const
		{
			return read(static_cast<void*>(buf), sz);
		}

		template <typename C>
		ssize_t read(fmt::basic_string<C>& u) const
		{
			return read(u.data(), u.size() * sizeof (C));
		}

		template <typename C>
		ssize_t read(fmt::vector<C>& t) const
		{
			return read(t.data(), t.size() * sizeof (C));
		}
	};

	struct writer
	{
		virtual ssize_t write(const void *buf, size_t sz) const = 0;

		template <typename C>
		ssize_t write(const C *buf, size_t sz = sizeof(C)) const
		{
			return write(static_cast<const void*>(buf), sz);
		}

		template <typename C>
		ssize_t write(fmt::basic_string_view<C> u) const
		{
			return write(u.data(), u.size() * sizeof (C));
		}

		template <typename C>
		ssize_t write(fmt::span<C> p) const
		{
			return write(p.data(), p.size() * sizeof (C));
		}
	};

	struct stream : reader, writer 
	{ };
}

#endif // file
