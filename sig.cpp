// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sig.hpp"
#include "err.hpp"
#include <csignal>
#include <map>

namespace
{
	sys::sig::subject &event(int no)
	{
		static std::map<int, sys::sig::subject> map;
		return map[no];
	}

	void send(int no)
	{
		event(no).send([no](auto const &p)
		{
			p.second(no);
		});
	}
}

namespace sys::sig
{
	scope::scope(int on, observer ob)
	: slot(&event(on), ob)
	{
		old.ob = std::signal(on, send);
		if (SIG_ERR == old.ob)
		{
			sys::err(here, "signal", on);
		}
	}

	scope::~scope()
	{
		if (SIG_ERR != old.ob)
		{
			(void) std::signal(old.on, old.ob);
		}
	}

	link::link(int on, observer ob) : scope(on, next(ob))
	{ }

	observer link::next(observer ob)
	{
		return [=](int on)
		{
			ob(on);
			old.ob(on);
		};
	}
}

