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
	using namespace env::file;

	using string = fmt::string;
	using view   = string::view;
	using vector = string::vector;
	using edges  = view::edges;
	using span   = view::span;
	using entry  = fwd::predicate<view>;

	constexpr auto stop = fwd::always<view>;
	constexpr auto next = fwd::never<view>;

	edges paths(); // pwd, paths
	edges config(); // config_home, config_dirs
	edges data(); // data_home, data_dirs

	entry mask(mode);
	entry regx(view);
	entry to(string &);
	entry to(vector &);
	entry all(view, mode = ok, entry = next);
	entry any(view, mode = ok, entry = stop);

	bool find(view, entry);
	bool find(span, entry);
	bool find(edges, entry);

	bool fail(view path, mode = ok);
	view make(view path);
	bool remove(view path);

	class tmp
	{
		view stem;

	public:

		tmp(view path)
		{
			stem = make(path);
		}

		~tmp()
		{
			if (not stem.empty())
			{
				remove(stem);
			}
		}
	};
}

#endif // file
