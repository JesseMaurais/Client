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

	bool getline(console, string &);
	bool where(view, entry);
	bool find(view, entry);
	
	inline bool with(console go, entry look)
	{
		string s;
		while (getline(go, s)) if (look(s)) return true;
		return false;
	}

	inline bool with(command line, entry look)
	{
		process go(line);
		return with(go, look);
	}
}

#endif // file
