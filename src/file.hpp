#ifndef file_hpp
#define file_hpp "File Object"

#include <cstddef>

namespace env::file
{
	constexpr int invalid = -1;

	inline bool fail(int value)
	{
		return invalid == value;
	}

	using size_t = std::size_t;
	using ssize_t = std::ptrdiff_t;

	struct reader
	{
		virtual ssize_t read(void *buf, size_t sz) const = 0;

		template <typename C>
		ssize_t read(C *buf, size_t sz = 1) const
		{
			return read(static_cast<void*>(buf), sz * sizeof (C));
		}
	};

	struct writer
	{
		virtual ssize_t write(const void *buf, size_t sz) const = 0;

		template <typename C>
		ssize_t write(const C *buf, size_t sz = 1) const
		{
			return write(static_cast<const void*>(buf), sz * sizeof (C));
		}
	};

	struct stream : reader, writer 
	{ };
}

#endif // file
