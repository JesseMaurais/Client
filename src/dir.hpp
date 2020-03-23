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
	string join(string::view::init);
}

namespace fmt::dir
{
	string::view::vector split(string::view);
	string join(string::view::span);
	string join(string::view::init);
}

namespace env::dir
{
	using fmt::string;
	using entry = predicate<string::view>;
	using order = constant<string::view::node>;

	extern order::ref paths; // pwd, paths
	extern order::ref config; // config_home, config_dirs
	extern order::ref data; // data_home, data_dirs

	constexpr auto stop = always<string::view>;
	constexpr auto next = never<string::view>;
	entry mask(file::mode);
	entry regx(string::view);
	entry to(string::ref);
	entry to(string::vector::ref);
	entry all(string::view, file::mode = file::ok, entry = next);
	entry any(string::view, file::mode = file::ok, entry = stop);

	bool find(string::view, entry);
	bool find(string::view::span, entry);
	bool find(string::span, entry);
	bool find(string::view::node, entry);

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
