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
}

#ifndef NDEBUG
void test::run<test::unit::sig>() noexcept
{
	std::vector<int> caught;
	std::vector<int> raised = { SIGINT, SIGFPE, SIGILL };

	for (int n : raised)
	{
		sys::sig::scope const after
		(
			n, [&](int on) 
			{ 
				caught.push_back(on); 
			}
		);
		std::raise(n);
	}

	for (int n : raised)
	{
		auto it = caught.find(raised.at(n));
		assert(caught.end() != it);
	}
}
#endif
