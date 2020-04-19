#ifndef desktop_hpp
#define desktop_hpp "Desktop Utility"

#include "ini.hpp"
#include "shell.hpp"

namespace env::desktop
{
	struct entry : doc::ini
	{
		bool got(word) const;
		view get(word) const;
		bool set(word, view);
		bool put(word, view);
	};

	bool is(string::view name);
	// Whether name of desktop session

	struct dialog : shell
	// Start a basic dialog type
	{
		pair get(span command);
		// Open dialog by args

		enum class msg
		{
			error, info, query, warn
		};
		pair message(msg, view text);
		// Show user a message box

		enum txt
		{
			none = 0, edit = 1, html = 2
		};
		pair text(view path, view check = "", txt = none, view font = "");
		// Show user the contents of a text file

		enum mode 
		{
			none = 0, many = 1, dir = 2, save = 4 
		};
		pair select(view start = "", mode = none);
		// User selects file(s) from the system

		pair enter(view start, view label = "", bool hide = false);
		// User enters text into an edit box

		pair form(edges add, view text = "", view title = "");
		// User enters data in a form

		pair notify(view text, view icon = "info");
		// Show the user a desktop notification

		pair calendar(view text = "", view format = "", int day = 0, int month = 0, int year);
		// User selects a calendar data

		pair color(view start = "", bool show_palette = true);
		// User selects a color value
	};
}

#endif // file
