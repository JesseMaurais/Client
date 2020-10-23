#ifndef desktop_hpp
#define desktop_hpp "Desktop Utility"

#include "shell.hpp"
#include "opt.hpp"

namespace env
{
	extern struct desktop : shell
	{
		static bool current(view name);
		// Whether name matches current session

		line with(string::span command);
		// Open dialog with command

		line open(view application);
		// Start named application

		line notify(view text, view icon = "info");
		// Show the user a desktop notification

		line calendar(view text = "", view format = "", int day = 0, int month = 0, int year = 0);
		// User selects a calendar data

		line color(view start = "", bool palette = true);
		// User selects a color value

		line enter(view start, view label = "", bool hide = false);
		// User enters text into an edit box

		using controls = fwd::span<fwd::pair<view>>;
		line form(controls, view text = "", view title = "");
		// User enters data into a form

		enum class msg { info, query, warn, error };
		line message(view text, msg = msg::info);
		// Show a message box with buttons

		enum class txt { none, edit, html };
		line text(view path, view check = "", view font = "", txt = txt::none);
		// Show text file contents (like a EULA with checkbox)

		enum class mode { none, many, dir, save };
		line select(view start = "", mode = mode::none);
		// User selects file(s) from the system

	} dialog;
}

#endif // file
