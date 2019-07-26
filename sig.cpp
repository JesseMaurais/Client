// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sig.hpp"
#include "err.hpp"
#include "dbg.hpp"
#include <csignal>
#include <map>

namespace
{
	sys::sig::subject &event(int signo)
	{
		static std::map<int, sys::sig::subject> map;
		return map[signo];
	}

	void raise(int no)
	{
		// Insecure but portable persistence
		verify(SIG_ERR != std::signal(no, raise));

		event(no).send([no](auto const &p)
		{
			p.second(no);
		});
	}
}

namespace sys::sig
{
	slot::slot(int on, observer ob)
	: base(&event(on), ob)
	{
		old.ob = std::signal(on, raise);
		if (SIG_ERR == old.ob)
		{
			sys::err(here, "signal", on);
		}
		else
		{
			old.ob = ob;
		}
	}

	slot::~slot()
	{
		verify(raise == std::signal(old.on, old.ob));
	}

	slot::operator bool()
	{
		return SIG_ERR != old.ob;
	}

	link::link(int on, observer ob) : slot(on, next(ob))
	{ }

	observer link::next(observer ob)
	{
		return [=](int on)
		{
			ob(on);
			old.ob(oo);
		};
	}
}

