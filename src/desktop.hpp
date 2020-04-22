#ifndef desktop_hpp
#define desktop_hpp "Desktop Utility"

#include "shell.hpp"

namespace env::desktop
{
	using fmt::string;
	using string::view;

	bool got(view); // entry exists
	view get(view); // entry value
	bool set(view, view); // copy to entry
	bool put(view, view); // view in entry

	bool is(string::view name);
	// Whether desktop session

	struct dialog : shell
	{
		subspan get(span command);
		// Open dialog by command

		enum class msg
		{
			error, info, query, warn
		};
		subspan message(msg, view text);
		// Show a message box with buttons

		enum txt
		{
			none = 0, edit = 1, html = 2
		};
		subspan text(view path, view check = "", view font = "", txt = none);
		// Show text file contents (like a EULA with checkbox)

		enum sel
		{
			none = 0, many = 1, dir = 2, save = 4 
		};
		subspan select(view start = "", sel = none);
		// User selects file(s) from the system

		subspan enter(view start, view label = "", bool hide = false);
		// User enters text into an edit box

		subspan form(edges add, view text = "", view title = "");
		// User enters data in a form

		subspan notify(view text, view icon = "info");
		// Show the user a desktop notification

		subspan calendar(view text = "", view format = "", int day = 0, int month = 0, int year);
		// User selects a calendar data

		subspan color(view start = "", bool palette = true);
		// User selects a color value
	};
}

#endif // file
