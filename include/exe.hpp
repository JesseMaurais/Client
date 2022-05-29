#ifndef exe_hpp
#define exe_hpp "Execute Command Line"

#include "fmt.hpp"

namespace env::exe
{
	fmt::vector get(fmt::input in, char end = '\n', int count = -1);
	// Cache an amount of delimited input lines

	fmt::vector run(fmt::span args);
	// Run using vector view

	fmt::vector run(fmt::init args);
	// Run using initializers

	fmt::vector echo(fmt::view line);
	// Expand macros in line

	fmt::vector list(fmt::view directory = ".");
	// List files in a directory

	fmt::vector copy(fmt::view path);
	// Lines in file given by path

	fmt::vector find(fmt::view pattern, fmt::view directory = ".");
	// Paths to matching files in directory

	fmt::vector which(fmt::view name);
	// Paths to executables with program name

	fmt::vector start(fmt::view path);
	// Preferred application for file type at path

	fmt::vector imports(fmt::view path);
	// Dump imports in binary at path

	fmt::vector exports(fmt::view path);
	// Dump exports in binary at path

	bool desktop(fmt::view name);
	// Whether name matches current session

	fmt::vector dialog(fmt::span command);
	// Open dialog with command

	fmt::vector notify(fmt::view text, fmt::view icon = "info");
	// Show the user a desktop notification

	fmt::vector calendar(fmt::view text = "", fmt::view format = "", int day = 0, int month = 0, int year = 0);
	// User selects a calendar data

	fmt::vector color(fmt::view start = "", bool palette = true);
	// User selects a color value

	fmt::vector enter(fmt::view start, fmt::view label = "", bool hide = false);
	// User enters text into an edit box

	using controls = fwd::span<fmt::pair>;
	fmt::vector form(controls, fmt::view text = "", fmt::view title = "");
	// User enters data into a form

	enum class msg { info, query, warn, error };
	fmt::vector show(fmt::view text, msg = msg::info);
	// Show a message box with buttons

	enum class txt { none, edit, html };
	fmt::vector text(fmt::view path, fmt::view check = "", fmt::view font = "", txt = txt::none);
	// Show text file contents (like a EULA with checkbox)

	enum class mode { none, many, dir, save };
	fmt::vector select(fmt::view start = "", mode = mode::none);
	// User selects file(s) from the system
}

#endif // file
