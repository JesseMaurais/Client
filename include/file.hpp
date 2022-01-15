#ifndef file_hpp
#define file_hpp "File Object"

#include "fwd.hpp"
#include "mode.hpp"
#include "ptr.hpp"
#include <cstddef>
#include <cstdio>

namespace env::file
{
	using basic_ptr = fwd::as_ptr<FILE>;
	using shared_ptr = fwd::shared_ptr<FILE>;
	using unique_ptr = fwd::unique_ptr<FILE>;
	using weak_ptr = std::weak_ptr<FILE>;

	inline bool fail(basic_ptr ptr)
	{
		return nullptr == ptr;
	}

	unique_ptr enclose(basic_ptr);
	unique_ptr open(string::view, mode=rd);
	bool lock(basic_ptr, mode=wo, size_t=0, size_t=0);

	inline auto temp()
	{
		auto f = std::tmpfile();
		return make(f);
	}

	constexpr int invalid = -1;

	inline bool fail(auto value)
	{
		return invalid == value;
	}

	using size_t = std::size_t;
	using ssize_t = std::ptrdiff_t;
	using string = fmt::string;

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
}

#endif // file
