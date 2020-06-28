// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "sync.hpp"
#include "type.hpp"
#include <cassert> // overwrite assert

namespace sys
{
	thread_local fmt::string::view thread_id;

	bool debug =
	#ifdef NDEBUG
		false;
	#else
		true;
	#endif

	thread_local fmt::string::stream local;
	fmt::string::out::ref out = local;

	fmt::string::out::ref flush(fmt::string::out::ref put) 
	{
		static sys::mutex key;
		auto const unlock = key.lock();

		fmt::string line;
		while (std::getline(local, line))
		{
			put << line << std::endl;
		}
		return put;
	}

	int impl::bug(fmt::string::view message, bool no)
	{
		thread_local struct
		{
			fmt::string last;
			int counter = -1;
		} local;
		// Avoid spamming
		if (message != local.last)
		{
			// reset
			local.counter = 0;
			local.last = fmt::to_string(message);
			// format
			{
				// message
				out << local.last;
				// number
				if (no)
				{
					out << ':' << ' ' << std::strerror(errno);
				}
				// thread
				if (not empty(thread_id))
				{
					out << ' ' << '[' << thread_id << ']';
				}
			}
			out << fmt::eol;
		}
		else ++local.counter;
		return local.counter;
	}
}

