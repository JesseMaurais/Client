#include "sig.hpp"
#include "err.hpp"
#include "dbg.hpp"
#include <csignal>
#include <cassert>
#include <map>

namespace
{
	sys::sig::subject &event(int signo)
	{
		static std::map<int, sys::sig::subject> map;
		return map[signo];
	}

	void send(int signo)
	{
		event(signo).send([signo](auto const &pair)
		{
			pair.second(signo);
		});
	}
}

namespace sig::sys
{
	slot::slot(int no, sys::sig::observer fn)
	: base(&::event(no), fn)
	{
		old.fn = std::signal(no, ::send);
		if (SIG_ERR == old.fn)
		{
			sys::perror("signal", no);
		}
		else
		{
			old.no = no;
		}
	}

	slot::~slot()
	{
		verify(::send == std::signal(old.no, old.fn));
	}

	slot::operator bool()
	{
		return SIG_ERR != old.fn;
	}
}

