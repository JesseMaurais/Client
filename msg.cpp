#include <vector>
#include "msg.hpp"

#ifdef _WIN32
# include "win.hpp"
#else
# include <csignal>
# include <poll.h>
#endif

namespace sys::msg
{
	struct poll::impl
	{
		#ifdef _WIN32
		std::vector<HANDLE> handles;
		#else
		std::vector<pollfd> pollfds;
		#endif
		std::vector<receiver> slots;
	};

	poll() : ptr = std::make_unique<impl>() { }

	bool poll::wait(int timeout)
	{
		#ifdef _WIN32
		{
			auto const ms = fail(timeout) ? INFINITE : timeout;
			auto const dw = WaitForMultipleObjects
			(
				ptr->handles.size(),
				ptr->handles.data(),
				false,
				ms
			);

			switch (dw)
			{
			case WAIT_FAILED:
				sys::win::err(here, "WaitForMultipleObjects");
				[[fallthrough]];
			case WAIT_TIMEOUT:
				return true;
			default:
				if (dw < WAIT_ABANDONED_0)
				{
					
				}
				else
				{

				}
			}
			return false;
		}
		#else
		{
			auto& fds = ptr->pollfds;
			auto n = poll(fds.data(), fds.size(), timeout);
			if (0 < n)
			{
				auto& slots = ptr->slots;
				for (int i = 0; 0 < n; ++i)
				{
					if (auto ev = fds[i].revents)
					{
						slots[i](ev);
						-- n;
					}
				}
				return false;
			}
			else
			if (fail(n))
			{
				sys::err(here);
			}
			return true;
		}
		#endif
	}
}

