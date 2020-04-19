#ifndef desktop_hpp
#define desktop_hpp "Desktop Utility"


#include "ini.hpp"
#include "dir.hpp"

namespace env::desktop
{
	struct entry : doc::ini
	{
		bool got(word) const;
		view get(word) const;
		bool set(word, view);
		bool put(word, view);
	};

	entry find(dir::entry);

	bool is(string::view name);
	// Whether name of desktop session

	struct dialog : file::process
	// Start a basic dialog type
	{
		explicit dialog(string::view::span args);

	private:

		using process::process;
	};
}

#endif // file
