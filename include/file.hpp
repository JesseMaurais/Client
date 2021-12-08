#ifndef file_hpp
#define file_hpp "File Object"

#include "fwd.hpp"
#include "ptr.hpp"
#include <cstddef>
#include <cstdio>

namespace env::file
{
	constexpr int invalid = -1;

	inline bool fail(auto value)
	{
		return invalid == value;
	}

	using size_t = std::size_t;
	using ssize_t = std::ptrdiff_t;

	struct reader
	{
		virtual ssize_t read(fwd::as_ptr<void> buf, size_t sz) const = 0;

		template <class C> auto read(fwd::as_ptr<C> buf, size_t sz = 1) const
		{
			return read(fwd::cast_as<void>(buf), sz * sizeof (C));
		}
	};

	struct writer
	{
		virtual ssize_t write(fwd::as_ptr<const void> buf, size_t sz) const = 0;

		template <class C> auto write(fwd::as_ptr<const C> buf, size_t sz = 1) const
		{
			return write(fwd::cast_as<const void>(buf), sz * sizeof (C));
		}
	};

	using stream = fwd::compose<reader, writer>;
	

	fwd::extern_ptr<FILE> close(fwd::as_ptr<FILE>);
	
	fwd::destroy<int[2]> pipe();

	fwd::destroy<int[3]> popen(fmt::init);
}

#endif // file
