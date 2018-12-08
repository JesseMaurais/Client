#include "env.hpp"
#include "dbg.hpp"
#include "sys.hpp"
#include "os.hpp"

#if __has_include(<winbase.h>)
#include <winbase.h>
#endif

namespace sys::env
{
	std::string_view get(std::string_view s)
	{
		return std::getenv(s.data());
	}

	bool put(std::string_view s)
	{
		return sys::putenv(s.data());
	}
}

namespace sys
{
	std::span<std::string_view> environ()
	{
		static std::vector<std::string_view> span;
		span.clear();

		if constexpr (sys::POSIX)
		{
			extern char **environ;
			if (environ)
			{
				for (auto var = environ; *var; ++var)
				{
					span.push_back(*var);
				}
			}
		}
		else
		if constexpr (sys::WIN32)
		{
			static struct strings
			{
				LPCH p;
				operator LPCH() { return p; }
				strings(LPCH p = NULL) { this->p = p; }
				~strings()
				{
					if (p and not ::FreeEnvironmentStringsA(p))
					{
						// GetLastError
					}
				}

			} environ;

			environ = ::GetEnvironmentStringsA();
			if (environ)
			{
				for (auto var = environ; *var; ++var)
				{
					std::string_view view(var);
					span.push_back(view);
					var += view.size();
				}
			}
		}

		return span;
	}
}

namespace env
{
	std::string_view user()
	{
		if constexpr (sys::POSIX)
		{
			return sys::env::get("USER");
		}
		else
		if constexpr (sys::WIN32)
		{
			return sys::env::get("USERNAME");
		}
	}

	std::string_view home()
	{
		if constexpr (sys::POSIX)
		{
			return sys::env::get("HOME");
		}
		else
		if constexpr (sys::WIN32)
		{
			return sys::env::get("USERPROFILE");
		}
	}

	std::string_view temp()
	{
		for (auto var : { "TEMP", "TMP", "TEMPDIR" })
		{
			auto view = sys::env::get(var);
			if (not view.empty())
			{
				return view;
			}
		}
		return "";
	}

	std::string_view shell()
	{
		if constexpr (sys::POSIX)
		{
			return sys::env::get("SHELL");
		}
		else
		if constexpr (sys::WIN32)
		{
			return sys::env::get("COMSPEC");
		}
	}
}
