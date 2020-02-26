#ifndef cli_hpp
#define cli_hpp

#include <initializer_list>
#include <utility>
#include "str.hpp"
#include "dir.hpp"
#include "ipc.hpp"

namespace env::cli
{
	using command = std::initializer_list<fmt::view>;
	using console = std::pair<int, int[3]>;

	console start(command);
	bool getline(console, string &);
	bool where(view, entry);
	bool find(view, entry);
}

namespace env::run
{
	bool with(cli::console, entry);
	bool with(cli::command, entry);
}

#endif // file
