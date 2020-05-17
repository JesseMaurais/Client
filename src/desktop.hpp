#ifndef desktop_hpp
#define desktop_hpp "Desktop Utility"

#include "shell.hpp"

namespace env::desktop
{
	using string = fmt::string;
	using view = string::view;
	using span = view::span;
	using edges = view::edges;

	bool got(view); // entry exists
	view get(view); // entry value
	bool set(view, view); // copy to entry
	bool put(view, view); // view in entry

	bool current(view name);
	// Whether named desktop

	struct dialog : shell
	{
		line get(span command);
		// Open dialog by command

		line open(view application);
		// Start named application

		line enter(view start, view label = "", bool hide = false);
		// User enters text into an edit box

		line form(edges add, view text = "", view title = "");
		// User enters data in a form

		line notify(view text, view icon = "info");
		// Show the user a desktop notification

		line calendar(view text = "", view format = "", int day = 0, int month = 0, int year = 0);
		// User selects a calendar data

		line color(view start = "", bool palette = true);
		// User selects a color value

		enum class msg { info, query, warn, error };
		line message(view text, msg = msg::info);
		// Show a message box with buttons

		enum class txt { none, edit, html };
		line text(view path, view check = "", view font = "", txt = txt::none);
		// Show text file contents (like a EULA with checkbox)

		enum class mode { none, many, dir, save };
		line select(view start = "", mode = mode::none);
		// User selects file(s) from the system
	};
}

#endif // file
