// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sig.hpp"
#include "type.hpp"
#include "err.hpp"
#include <memory>
#include <map>

namespace sys::sig
{
	socket* state::event(int n)
	{
		static std::map<int, sys::sig::socket> map;
		return std::addressof(map[n]);
	}

	void state::raise(int no)
	{
		event(no)->raise([no](auto const &p)
		{
			p.second(no);
		});
	}

	fmt::string::view text(int n)
	{
		using namespace std::literals;

		switch (n)
		{
		case SIGABRT:
			return "abort"sv;

		#ifdef SIGALRM
		case SIGALRM:
			return "alarm"sv;
		#endif

		#ifdef SIGBUS
		case SIGBUS:
			return "violation"sv;
		#endif

		#ifdef SIGCHLD
		case SIGCHLD:
			return "child"sv;
		#endif

		#ifdef SIGCONT
		case SIGCONT:
			return "continue"sv;
		#endif

		case SIGFPE:
			return "float"sv;

		#ifdef SIGHUP
		case SIGHUP:
			return "hangup"sv;
		#endif

		case SIGILL:
			return "illegal"sv;

		case SIGINT:
			return "interrupt"sv;

		#ifdef SIGKILL
		case SIGKILL:
			return "kill"sv;
		#endif

		#ifdef SIGPIPE
		case SIGPIPE:
			return "pipe"sv;
		#endif

		#ifdef SIGQUIT
		case SIGQUIT:
			return "quit"sv;
		#endif

		#ifdef SIGSTOP
		case SIGSTOP:
			return "stop"sv;
		#endif

		case SIGSEGV:
			return "segmentation"sv;

		case SIGTERM:
			return "terminate"sv;

		#ifdef SIGTSTP
		case SIGTSTP:
			return "stopping"sv;
		#endif

		#ifdef SIGTIN
		case SIGTIN:
			return "reading"sv;
		#endif

		#ifdef SIGTOU
		case SIGTOU:
			return "writing"sv;
		#endif

		#ifdef SIGUSR1
		case SIGUSR1:
			return "user(1)"sv;
		#endif

		#ifdef SIGUSR2
		case SIGUSR2:
			return "user(2)"sv;
		#endif

		#ifdef SIGPOLL
		case SIGPOLL:
			return "poll"sv;
		#endif

		#ifdef SIGPROF
		case SIGPROF:
			return "profile"sv;
		#endif

		#ifdef SIGPWR
		case SIGPWR:
			return "power"sv;
		#endif

		#ifdef SIGSTKFLT
		case SIGSTKFLT:
			return "stack fault"sv;
		#endif

		#ifdef SIGSYS
		case SIGSYS:
			return "system"sv;
		#endif

		#ifdef SIGTRAP
		case SIGTRAP:
			return "trap"sv;
		#endif

		#ifdef SIGURG
		case SIGURG:
			return "urgent"sv;
		#endif

		#ifdef SIGVTALRM
		case SIGVTALRM:
			return "virtual"sv;
		#endif

		#ifdef SIGWINCH
		case SIGWINCH:
			return "window"sv;
		#endif

		#ifdef SIGXCPU
		case SIGXCPU:
			return "time limit exceeded"sv;
		#endif

		#ifdef SIGXFSZ
		case SIGXFSZ:
			return "file limit exceeded"sv;
		#endif
		}
		
		thread_local fmt::string buf;
		buf = fmt::to_string(n);
		return buf;
	}
}

#ifdef test_unit
test_unit(sig)
{
	std::vector<int> caught;
	std::vector<int> raised = { SIGINT, SIGFPE, SIGILL };

	for (int n : raised)
	{
		sys::sig::state const after
		(
			n, [&](int n)
			{
				caught.push_back(n);
			}
		);
		std::raise(n);
	}

	auto const begin = caught.begin();
	auto const end = caught.end();

	for (int signo : raised)
	{
		assert(end != std::find(begin, end, signo));
	}
}
#endif