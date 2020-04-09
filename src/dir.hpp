#ifndef dir_hpp
#define dir_hpp "File Directory"

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
	using string::view;
	using string::vector;
	using view::span;
	using view::node;
	using file::mode;

	using entry = predicate<view>;

	extern env::node::ref paths; // pwd, paths
	extern env::node::ref config; // config_home, config_dirs
	extern env::node::ref data; // data_home, data_dirs

	constexpr auto stop = always<view>;
	constexpr auto next = never<view>;

	entry mask(mode);
	entry regx(view);
	entry to(string::ref);
	entry to(vector::ref);
	entry all(view, mode = file::ok, entry = next);
	entry any(view, mode = file::ok, entry = stop);

	bool find(view, entry);
	bool find(span, entry);
	bool find(span, entry);
	bool find(node, entry);

	bool fail(view path, mode = file::ok);
	view make(view path);
	bool remove(view path);

	class tmp
	{
		view stem;

	public:

		tmp(view path)
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
