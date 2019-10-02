#ifndef run_hpp
#define run_hpp

#include <initializer_list>
#include <utility>
#include "str.hpp"
#include "dir.hpp"
#include "ipc.hpp"

namespace env::run
{
	using command = std::initializer_list<fmt::view>;
	using console = std::pair<int, int[3]>;

	console start(command);
	void close(console);
	bool getline(console, string &);
	bool where(view, entry);
	bool find(view, entry);

	bool with(console, entry);
	bool with(command, entry);
}

#endif // file
