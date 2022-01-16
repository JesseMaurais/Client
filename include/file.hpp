#ifndef file_hpp
#define file_hpp "File System"

#include <cstdio>
#include "fmt.hpp"
#include "mode.hpp"
#include "ptr.hpp"

namespace env::file
{
	using size_t = std::size_t;
	using ssize_t = std::ptrdiff_t;
	using string = fmt::string;
	using view = string::view;

	using basic_ptr = fwd::as_ptr<FILE>;
	using unique_ptr = fwd::unique_ptr<FILE>;
	using shared_ptr = fwd::shared_ptr<FILE>;
	using weak_ptr = std::weak_ptr<FILE>;

	using basic_buf = fwd::as_ptr<char>;
	using unique_buf = fwd::unique_ptr<char>;
	using shared_buf = fwd::shared_ptr<char>;
	using weak_buf = std::weak_ptr<char>;

	unique_ptr enclose(basic_ptr);
	unique_ptr open(view, mode=rw);

	unique_ptr lock(basic_ptr, mode=rw, off_t=0, size_t=0);
	unique_buf map(basic_ptr, mode=rw, off_t=0, size_t=0, basic_buf=nullptr);

	inline auto temp()
	{
		auto f = std::tmpfile();
		return enclose(f);
	}
}

#endif // file
