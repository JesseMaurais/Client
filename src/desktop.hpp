#ifndef desktop_hpp
#define desktop_hpp

#include "str.hpp"
#include "env.hpp"
#include "process.hpp"

namespace env::desktop
{
	bool is(fmt::string::view name);
	/// Whether $name of desktop session

	struct dialog : env::file::process
	/// Start a basic dialog type
	{
		explicit dialog(fmt::string::view::span args);

	private:

		using process::process;
	};
}

#endif // file
