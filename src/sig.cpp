// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sig.hpp"
#include "err.hpp"
#include "type.hpp"
#ifdef _WIN32
#include "win.hpp"
#else
#include "uni.hpp"
#endif
#include <map>

namespace sys::sig
{
	socket &scope::event(int no)
	{
		static std::map<int, sys::sig::socket> map;
		return map[no];
	}

	void scope::raise(int no)
	{
		event(no).raise([no](auto const &p)
		{
			p.second(no);
		});
	}

	fmt::string to_string(int signo)
	{
		switch (signo)
		{
		case SIGABRT:
			return "Abort";

		#ifdef SIGALRM
		case SIGALRM:
			return "Alarm";
		#endif

		#ifdef SIGBUS
		case SIGBUS:
			return "Memory access violation";
		#endif

		#ifdef SIGCHLD
		case SIGCHLD:
			return "Child process stopped/continued";
		#endif

		#ifdef SIGCONT
		case SIGCONT:
			return "Continue processing";
		#endif

		case SIGFPE:
			return "Float-point error";

		#ifdef SIGHUP
		case SIGHUP:
			return "Hang up";
		#endif

		case SIGILL:
			return "Illegal instruction";

		case SIGINT:
			return "Interrupt";

		#ifdef SIGKILL
		case SIGKILL:
			return "Kill";
		#endif

		#ifdef SIGPIPE
		case SIGPIPE:
			return "Broken pipe";
		#endif

		#ifdef SIGQUIT
		case SIGQUIT:
			return "Quit processing";
		#endif

		case SIGSEGV:
			return "Segmentation fault";

		#ifdef SIGSTOP
		case SIGSTOP:
			return "Stop processing";
		#endif

		case SIGTERM:
			return "Terminate";

		#ifdef SIGTSTP
		case SIGTSTP:
			return "Terminal stopped";
		#endif

		#ifdef SIGTIN
		case SIGTIN:
			return "Process reading";
		#endif

		#ifdef SIGTOU
		case SIGTOU:
			return "Process writing";
		#endif

		#ifdef SIGUSR1
		case SIGUSR1:
			return "User defined (1)";
		#endif

		#ifdef SIGUSR2
		case SIGUSR2:
			return "User defined (2)";
		#endif

		#ifdef SIGPOLL
		case SIGPOLL:
			return "Poll event";
		#endif

		#ifdef SIGPROF
		case SIGPROF:
			return "Profiling timer expired";
		#endif

		#ifdef SIGPWR
		case SIGPWR:
			return "Power limited";
		#endif

		#ifdef SIGSTKFLT
		case SIGSTKFLT:
			return "Stack fault";
		#endif

		#ifdef SIGSYS
		case SIGSYS:
			return "Bad system call";
		#endif

		#ifdef SIGTRAP
		case SIGTRAP:
			return "Trace or breakpoint trap";
		#endif

		#ifdef SIGURG
		case SIGURG:
			return "Urgent out of band data";
		#endif

		#ifdef SIGVTALRM
		case SIGVTALRM:
			return "Virtual timer expired";
		#endif

		#ifdef SIGWINCH
		case SIGWINCH:
			return "Window resized";
		#endif

		#ifdef SIGXCPU
		case SIGXCPU:
			return "CPU time limit exceeded";
		#endif

		#ifdef SIGXFSZ
		case SIGXFSZ:
			return "File size limit exceeded";
		#endif

		default:
			return fmt::to_string(signo);
		}

	}
}

#ifdef test_unit
test_unit(sig)
{
	std::vector<int> caught;
	std::vector<int> raised = { SIGINT, SIGFPE, SIGILL };

	for (int signo : raised)
	{
		sys::sig::scope const after
		(
			signo, [&caught](int signo) 
			{ 
				caught.push_back(signo); 
			}
		);
		std::raise(signo);
	}

	auto const begin = caught.begin();
	auto const end = caught.end();

	for (int signo : raised)
	{
		assert(end != std::find(begin, end, signo));
	}
}
#endif

