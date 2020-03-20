#ifndef dir_hpp
#define dir_hpp

#include "fmt.hpp"
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
	using fmt::string;
	using entry = predicate<string::view>;
	using pair = std::pair<string::view, string::view::span>;

	entry mask(file::mode);
	entry regx(string::view);
	entry to(string::ref);
	entry to(string::vector::ref);
	constexpr auto stop = always<string::view>;
	constexpr auto skip = never<string::view>;

	static auto
		ex = mask(file::ex),
		wr = mask(file::wr),
		rd = mask(file::rd),
		ok = mask(file::ok),
		rw = mask(file::rw),
		rwx = mask(file::rwx);

	inline auto all(string::view u, file::mode m = file::ok, entry e = skip)
	{
		return mask(m) || regx(u) || e;
	}

	inline auto any(string::view u, file::mode m = file::ok)
	{
		return all(u, m, stop);
	}

	extern env::pair::ref paths;
	extern env::pair::ref config;
	extern env::pair::ref data;

	bool find(string::view::span, entry);
	bool find(string::view, entry);
	bool find(pair, entry);

	bool fail(string::view path, file::mode = file::ok);
	string::view make(string::view path);
	bool remove(string::view path);

	class tmp
	{
		string::view stem;

	public:

		tmp(string::view path)
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
