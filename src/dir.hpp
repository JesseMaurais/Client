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

namespace env::file
{
	using entry  = fwd::predicate<fmt::string::view>;

	constexpr auto stop = fwd::always<fmt::string::view>;
	constexpr auto next = fwd::never<fmt::string::view>;

	fmt::string::view::edges paths(); // pwd, paths
	fmt::string::view::edges config(); // config_home, config_dirs
	fmt::string::view::edges data(); // data_home, data_dirs

	bool find(fmt::string::view, entry);
	// Iterate directory until predicate
	bool find(fmt::string::view::span, entry);
	bool find(fmt::string::view::edges, entry);

	entry mask(env::file::mode);
	entry regx(fmt::string::view);
	entry to(fmt::string &);
	entry to(fmt::string::vector &);
	entry all(fmt::string::view, mode = ok, entry = next);
	entry any(fmt::string::view, mode = ok, entry = stop);

	fmt::string::view make_dir(fmt::string::view path);
	bool remove_dir(fmt::string::view path);
}

#endif // file
