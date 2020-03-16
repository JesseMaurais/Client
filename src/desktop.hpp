#ifndef desktop_hpp
#define desktop_hpp

#include "str.hpp"
#include "process.hpp"

namespace env::desktop
{
	bool is(fmt::string_view name);
	/// Whether $name of desktop session

	fmt::vector_string which(fmt::string_view name);
	/// Paths to executables with program $name

	struct open : env::file::process
	/// Preferred application at $path
	{
		explicit open(fmt::string_view path);

	private:

		using process::process;
	};

	struct dialog : env::file::process
	/// Start a basic dialog type
	{
		explicit dialog(fmt::span_view args);

	private:

		using process::process;
	};
}

#endif // file
