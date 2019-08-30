#ifndef ops_hpp
#define ops_hpp

#include <cstddef>
#include <cstdint>

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
		virtual ssize_t read(void* buf, size_t sz) const = 0;
		virtual ssize_t write(const void* buf, size_t sz) const = 0;

		template <typename C>
		ssize_t read(C* buf, size_t sz) const
		{
			return read(static_cast<void*>(buf), sz);
		}

		template <typename C>
		ssize_t write(const C* buf, size_t sz) const
		{
			return write(static_cast<const void*>(buf), sz);
		}
	};
}

#endif // file

