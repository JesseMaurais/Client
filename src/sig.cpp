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
		case SIGILL:
			return "Illegal instruction";
		case SIGINT:
			return "Interruption";
		case SIGFPE:
			return "Float-point error";
		case SIGABRT:
			return "Abort";
		case SIGTERM:
			return "Terminate";
		case SIGSEGV:
			return "Segmentation fault";
		#ifdef SIGKILL
		case SIGKILL:
			return "Killed";
		#endif
		default:
			return fmt::to_string(signo);
		}

	}
}

#ifdef test
test(sig)
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

