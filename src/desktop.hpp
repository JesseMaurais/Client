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

	bool current(view name);
	// Whether named desktop

	struct dialog : shell
	{
		line get(line command);
		// Open dialog by command

		line open(view application);
		// Start named application

		line edit(view start, view label = "", bool hide = false);
		// User enters text into an edit box

		line form(edges add, view text = "", view title = "");
		// User enters data in a form

		line notify(view text, view icon = "info");
		// Show the user a desktop notification

		line calendar(view text = "", view format = "", int day = 0, int month = 0, int year);
		// User selects a calendar data

		line color(view start = "", bool palette = true);
		// User selects a color value

		enum class msg
		{
			error, info, query, warn
		};
		line message(msg, view text);
		// Show a message box with buttons

		enum txt
		{
			none = 0, edit = 1, html = 2
		};
		line text(view path, view check = "", view font = "", txt = none);
		// Show text file contents (like a EULA with checkbox)

		enum sel
		{
			none = 0, many = 1, dir = 2, save = 4 
		};
		line select(view start = "", sel = none);
		// User selects file(s) from the system
	};
}

#endif // file
