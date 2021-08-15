#ifndef cmd_hpp
#define cmd_hpp "Command Line Shell"

#include "fmt.hpp"

namespace env
{
	struct shell
	{
		using string = fmt::string;
		using view   = string::view;
		using vector = string::vector;
		using span   = view::span;
		using init   = view::init;
		using page   = string::page;
		using out    = string::out::ref;
		using in     = string::in::ref;

		fmt::string::vector cache;
		string last;
		int status;

		page get(in, char end = '\n', int count = 0);
		// Cache all lines in to end, return indices

		page run(init arguments);
		// Run command as sub process

		page run(span arguments);
		// Run command as sub process

		page echo(view line);
		// Expand macros in line

		page list(view directory = ".");
		// List files in directory

		page copy(view path);
		// Lines in file given by path

		page find(view pattern, view directory = ".");
		// Paths to matching files in directory

		page which(view name);
		// Paths to executables with program name

		page open(view path);
		// Preferred application for file type at path

		page imports(view path);
		// Dump imports in binary at path

		page exports(view path);
		// Dump exports in binary at path

		static bool desktop(view name);
		// Whether name matches current session

		page with(string::span command);
		// Open dialog with command

		page notify(view text, view icon = "info");
		// Show the user a desktop notification

		page calendar(view text = "", view format = "", int day = 0, int month = 0, int year = 0);
		// User selects a calendar data

		page color(view start = "", bool palette = true);
		// User selects a color value

		page enter(view start, view label = "", bool hide = false);
		// User enters text into an edit box

		using controls = fwd::span<fwd::pair<view>>;
		page form(controls, view text = "", view title = "");
		// User enters data into a form

		enum class msg { info, query, warn, error };
		page message(view text, msg = msg::info);
		// Show a message box with buttons

		enum class txt { none, edit, html };
		page text(view path, view check = "", view font = "", txt = txt::none);
		// Show text file contents (like a EULA with checkbox)

		enum class mode { none, many, dir, save };
		page select(view start = "", mode = mode::none);
		// User selects file(s) from the system

	} &shell();
}

#endif // file
