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

		event(no).raise([no](auto const &p)
		{
			p.second(no);
		});
	}
}

namespace sys::sig
{
	slot::slot(int no, sys::sig::observer fn)
	: base(&event(no), fn)
	{
		old.fn = std::signal(no, send);
		if (SIG_ERR == old.fn)
		{
			sys::err(here, "signal", no);
		}
		else
		{
			old.no = no;
		}
	}

	slot::~slot()
	{
		verify(send == std::signal(old.no, old.fn));
	}

	slot::operator bool()
	{
		return SIG_ERR != old.fn;
	}

	link::link(int no, observer ob) : slot(no, next(ob))
	{ }

	observer link::next(observer ob)
	{
		return [=](int no)
		{
			ob(no);
			old.fn(no);
		};
	}
}

