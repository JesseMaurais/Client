#ifndef dir_hpp
#define dir_hpp "File Directory"

#include "fmt.hpp"
#include "tmp.hpp"
#include "mode.hpp"

namespace fmt::path
{
	vector split(view);
	string join(span);
	string join(init);
}

namespace fmt::dir
{
	vector split(view);
	string join(span);
	string join(init);
}

namespace fmt::file
{
	vector split(view);
	string join(span);
	string join(init);
}

namespace env::file
{
	using string = fmt::string;
	using view = fmt::view;
	using span = fmt::span;
	using order = fwd::pair<view, span>;
	using entry = fwd::predicate<view>;
	using notify = fwd::relation<view, mode>;
	using event = fwd::event;

	order paths(); // pwd, path
	order config(); // config_home, config_dirs
	order data(); // data_home, data_dirs

	// Iterate directory until predicate
	bool find(view, entry);
	bool find(span, entry);
	bool find(order, entry);

	constexpr auto stop = fwd::always<view>;
	constexpr auto next = fwd::never<view>;

	entry mask(mode);
	entry regex(view);
	entry to(string &);
	entry to(string::vector &);
	entry all(view, mode = ok, entry = next);
	entry any(view, mode = ok, entry = stop);

	// First extant as evaluted in path order
	string search(view, entry=mask(ok), order=paths());
	// Make path to directory, return extant root
	view mkdir(view);
	// Remove directory and all contents
	bool rmdir(view);
}

#endif // file
