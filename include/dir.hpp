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

namespace fmt::file
{
	string fifo(string::view);
}

namespace env::file
{
	using string = fmt::string;
	using view = string::view;
	using entry = fwd::predicate<view>;
	using event = fwd::relation<view, mode>;
	using edges = view::edges;
	using span = view::span;

	// Make path to directory, return extant root
	view mkdir(view);
	// Remove directory and all contents
	bool rmdir(view);

	constexpr auto stop = fwd::truth<view>;
	constexpr auto next = fwd::falsity<view>;

	edges paths(); // pwd, paths
	edges config(); // config_home, config_dirs
	edges data(); // data_home, data_dirs

	// Iterate directory until predicate
	bool find(view, entry);
	bool find(span, entry);
	bool find(edges, entry);

	entry mask(mode);
	entry regx(view);
	entry to(string &);
	entry to(string::vector &);
	entry all(view, mode = ok, entry = next);
	entry any(view, mode = ok, entry = stop);
}

#endif // file
