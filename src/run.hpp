#ifndef run_hpp
#define run_hpp

#include "str.hpp"
#include "dir.hpp"
#include "ipc.hpp"

namespace env::run
{
	extern bool const cmd;
	extern bool const sh;

	using file::process;
	using file::command;
	using dir::entry;
	using fmt::string;
	using fmt::view;

	bool getline(process &, string &);
	bool self(command, entry);
	bool where(view, entry);
	bool find(view, entry);
	
	inline bool with(process& go, entry look)
	{
		string s;
		while (getline(go, s)) if (look(s)) return true;
		return false;
	}
	
	inline bool with(console tty, entry look)
	{
		viewer go(tty);
		return with(go, look);
	}

	inline bool with(command line, entry look)
	{
		process go(line);
		return with(go, look);
	}
}

#endif // file
