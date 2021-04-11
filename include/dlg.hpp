#ifndef dlg_hpp
#define dlg_hpp "Desktop Utility Dialogs"

#include "cmd.hpp"
#include "fwd.hpp"

namespace env
{
	struct desktop : shell
	{
		static bool current(view name);
		// Whether name matches current session

		page with(string::span command);
		// Open dialog with command

		page open(view application);
		// Start named application

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

	desktop & dlg();
}

#endif // file
