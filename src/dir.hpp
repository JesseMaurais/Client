#ifndef dir_hpp
#define dir_hpp

#include "str.hpp"
#include "tmp.hpp"
#include "file.hpp"

namespace fmt::path
{
	array split(view);
	string join(div);
}

namespace fmt::dir
{
	array split(view);
	string join(div);
}

namespace env::dir
{
	using entry = predicate<fmt::view>;

	entry mask(file::mode);
	entry regx(fmt::view);
	entry to(fmt::string &);
	entry to(fmt::alloc &);

	constexpr auto stop = always<fmt::view>;
	constexpr auto skip = never<fmt::view>;

	static auto // access
		ex = mask(file::ex),
		wr = mask(file::wr),
		rd = mask(file::rd),
		ok = mask(file::ok),
		rw = mask(file::rw),
		rwx = mask(file::rwx);

	inline auto all(fmt::view u, file::mode m = file::ok, entry e = skip)
	{
		return mask(m) || regx(u) || e;
	}

	inline auto any(fmt::view u, file::mode m = file::ok)
	{
		return all(u, m, stop);
	}

	bool path(entry); // pwd + paths
	bool config(entry); // config home + config dirs
	bool data(entry); // data home + data dirs
	bool cache(entry); // cache home

	bool find(fmt::view, entry);
	bool find(fmt::div, entry);

	bool fail(fmt::view path, file::mode = file::ok);
	fmt::view make(fmt::view path);
	bool remove(fmt::view path);

	class tmp
	{
		fmt::view stem;

	public:

		tmp(fmt::view path)
		{
			stem = env::dir::make(path);
		}

		~tmp()
		{
			if (not empty(stem))
			{
				env::dir::remove(stem);
			}
		}
	};
}

#endif // file
