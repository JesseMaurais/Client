#ifndef cmd_hpp
#define cmd_hpp "Command Line Shell"

#include "fmt.hpp"

namespace env
{
	struct shell
	{
		fmt::string buffer;
		fmt::vector cache;
		int status;

		fmt::page get(fmt::input, char = '\n', int = -1);
		// Cache all lines in to end, return indices

		fmt::page run(fmt::span args);
		// Run using vector fmt::view

		fmt::page run(fmt::init args);
		// Run using fmt::initializers

		fmt::page echo(fmt::view line);
		// Expand macros in line

		fmt::page list(fmt::view directory = ".");
		// List files in directory

		fmt::page copy(fmt::view path);
		// Lines in file given by path

		fmt::page find(fmt::view pattern, fmt::view directory = ".");
		// Paths to matching files in directory

		fmt::page which(fmt::view name);
		// Paths to executables with program name

		fmt::page start(fmt::view path);
		// Preferred application for file type at path

		fmt::page imports(fmt::view path);
		// Dump imports in binary at path

		fmt::page exports(fmt::view path);
		// Dump exports in binary at path

		static bool desktop(fmt::view name);
		// Whether name matches current session

		fmt::page dialog(fmt::span command);
		// Open dialog with command

		fmt::page notify(fmt::view text, fmt::view icon = "info");
		// Show the user a desktop notification

		fmt::page calendar(fmt::view text = "", fmt::view format = "", int day = 0, int month = 0, int year = 0);
		// User selects a calendar data

		fmt::page color(fmt::view start = "", bool palette = true);
		// User selects a color value

		fmt::page enter(fmt::view start, fmt::view label = "", bool hide = false);
		// User enters text into an edit box

		using controls = fwd::span<fwd::pair<fmt::view>>;
		fmt::page form(controls, fmt::view text = "", fmt::view title = "");
		// User enters data into a form

		enum class msg { info, query, warn, error };
		fmt::page message(fmt::view text, msg = msg::info);
		// Show a message box with buttons

		enum class txt { none, edit, html };
		fmt::page text(fmt::view path, fmt::view check = "", fmt::view font = "", txt = txt::none);
		// Show text file contents (like a EULA with checkbox)

		enum class mode { none, many, dir, save };
		fmt::page select(fmt::view start = "", mode = mode::none);
		// User selects file(s) from the system
	};
}

#endif // file
