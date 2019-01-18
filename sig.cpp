// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sig.hpp"
#include "err.hpp"
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

namespace sys::sig
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
		old.fn = std::signal(old.no, old.fn);
		//assert(::send == old.fn); // win32 fails
	}

	slot::operator bool()
	{
		return SIG_ERR != old.fn;
	}
}

