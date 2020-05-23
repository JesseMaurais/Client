// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sig.hpp"
#include "err.hpp"
#ifdef _WIN32
#include "win.hpp"
#else
#include "uni.hpp"
#endif
#include <map>

namespace
{
	auto value(int signo)
	{
		switch (signo)
		{
		case SIGILL:
			return "Illegal";
		case SIGINT:
			return "Interrupt";
		case SIGFPE:
			return "Float";
		case SIGABRT:
			return "Abort";
		case SIGTERM:
			return "Terminate";
		case SIGSEGV:
			return "Segmented";
		default:
			return "Unknown";
		}
	}
}

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

	void print(int signo)
	{
		sys::out << value(signo);
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

