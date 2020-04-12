#ifndef desktop_hpp
#define desktop_hpp "Desktop Utility"

#include "env.hpp"
#include "type.hpp"
#include "process.hpp"

namespace env::desktop
{
	bool is(string::view name);
	/// Whether $name of desktop session

	struct dialog : file::process
	/// Start a basic dialog type
	{
		explicit dialog(string::view::span args);

	private:

		using process::process;
	};
}

#endif // file
