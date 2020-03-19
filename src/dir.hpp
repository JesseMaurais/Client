#ifndef dir_hpp
#define dir_hpp

#include "str.hpp"
#include "env.hpp"
#include "tmp.hpp"
#include "mode.hpp"

namespace fmt::path
{
	string::view::vector split(string::view);
	string join(string::view::span);
	string join(string::view::list);
}

namespace fmt::dir
{
	string::view::vector split(string::view);
	string join(string::view::span);
	string join(string::view::list);
}

namespace env::dir
{
	using entry = predicate<fmt::string::view>;
	using pair = std::pair<fmt::string::view, fmt::string::span>;

	entry mask(file::mode);
	entry regx(fmt::string::view);
	entry to(fmt::string::ref);
	entry to(fmt::string::vector::ref);

	constexpr auto stop = always<fmt::string::view>;
	constexpr auto skip = never<fmt::string::view>;

	static auto // access
		ex = mask(file::ex),
		wr = mask(file::wr),
		rd = mask(file::rd),
		ok = mask(file::ok),
		rw = mask(file::rw),
		rwx = mask(file::rwx);

	inline auto all(fmt::string::view u, file::mode m = file::ok, entry e = skip)
	{
		return mask(m) || regx(u) || e;
	}

	inline auto any(fmt::string::view u, file::mode m = file::ok)
	{
		return all(u, m, stop);
	}

	extern env::pair::ref paths;
	extern env::pair::ref config;
	extern env::pair::ref data;

	bool find(fmt::string::view::span, entry);
	bool find(fmt::string::view, entry);
	bool find(pair, entry);

	bool fail(fmt::string::view path, file::mode = file::ok);
	fmt::string::view make(fmt::string::view path);
	bool remove(fmt::string::view path);

	class tmp
	{
		fmt::string::view stem;

	public:

		tmp(fmt::string::view path)
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
