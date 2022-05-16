#ifndef cmd_hpp
#define cmd_hpp "Command Line Shell"

#include "fmt.hpp"

namespace env
{
	struct shell
	{
		using string   = fmt::string;
		using view     = fmt::view;
		using vector   = view::vector;
		using span     = view::span;
		using init     = view::init;
		using page     = view::page;
		using out      = view::out::ref;
		using in       = view::in::ref;

		string buffer;
		vector cache;
		int status;

		page get(in, char = '\n', int = -1);
		// Cache all lines in to end, return indices

		page run(span args);
		// Run using vector view

		page run(init args);
		// Run using initializers

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

		page start(view path);
		// Preferred application for file type at path

		page imports(view path);
		// Dump imports in binary at path

		page exports(view path);
		// Dump exports in binary at path

		static bool desktop(view name);
		// Whether name matches current session

		page dialog(span command);
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
	};
}

#endif // file
