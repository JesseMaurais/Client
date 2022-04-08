// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sig.hpp"
#include "type.hpp"
#include "sync.hpp"
#include "tag.hpp"
#include "dig.hpp"
#include "err.hpp"
#include <memory>
#include <map>


namespace
{
	sys::sig::string sigstr(int n)
	{
		switch (n)
		{
		case SIGABRT:
			return "SIGABRT";

		#ifdef SIGALRM
		case SIGALRM:
			return "SIGALRM";
		#endif

		#ifdef SIGBUS
		case SIGBUS:
			return "SIGBUS";
		#endif

		#ifdef SIGCHLD
		case SIGCHLD:
			return "SIGCHLD";
		#endif

		#ifdef SIGCONT
		case SIGCONT:
			return "SIGCONT";
		#endif

		case SIGFPE:
			return "SIGFPE";

		#ifdef SIGHUP
		case SIGHUP:
			return "SIGHUP";
		#endif

		case SIGILL:
			return "SIGILL";

		case SIGINT:
			return "SIGINT";

		#ifdef SIGKILL
		case SIGKILL:
			return "SIGKILL";
		#endif

		#ifdef SIGPIPE
		case SIGPIPE:
			return "SIGPIPE";
		#endif

		#ifdef SIGQUIT
		case SIGQUIT:
			return "SIGQUIT";
		#endif

		#ifdef SIGSTOP
		case SIGSTOP:
			return "SIGSTOP";
		#endif

		case SIGSEGV:
			return "SIGSEGV";

		case SIGTERM:
			return "SIGTERM";

		#ifdef SIGTSTP
		case SIGTSTP:
			return "SIGTSTP";
		#endif

		#ifdef SIGTIN
		case SIGTIN:
			return "SIGTIN";
		#endif

		#ifdef SIGTOU
		case SIGTOU:
			return "SIGTOU";
		#endif

		#ifdef SIGUSR1
		case SIGUSR1:
			return "SIGUSR1";
		#endif

		#ifdef SIGUSR2
		case SIGUSR2:
			return "SIGUSR2";
		#endif

		#ifdef SIGPOLL
		case SIGPOLL:
			return "SIGPOLL";
		#endif

		#ifdef SIGPROF
		case SIGPROF:
			return "SIGPROF";
		#endif

		#ifdef SIGPWR
		case SIGPWR:
			return "SIGPWR";
		#endif

		#ifdef SIGSTKFLT
		case SIGSTKFLT:
			return "SIGSTKFLT";
		#endif

		#ifdef SIGSYS
		case SIGSYS:
			return "SIGSYS";
		#endif

		#ifdef SIGTRAP
		case SIGTRAP:
			return "SIGTRAP";
		#endif

		#ifdef SIGURG
		case SIGURG:
			return "SIGURG";
		#endif

		#ifdef SIGVTALRM
		case SIGVTALRM:
			return "SIGVTALRM";
		#endif

		#ifdef SIGWINCH
		case SIGWINCH:
			return "SIGWINCH";
		#endif

		#ifdef SIGXCPU
		case SIGXCPU:
			return "SIGXCPU";
		#endif

		#ifdef SIGXFSZ
		case SIGXFSZ:
			return "SIGXFSZ";
		#endif
		}

		return fmt::empty;
	}

	fwd::span<int> sigset()
	{
		static int buf [] =
		{
			SIGABRT,

			#ifdef SIGALRM
			SIGALRM,
			#endif

			#ifdef SIGBUS
			SIGBUS,
			#endif

			#ifdef SIGCHLD
			SIGCHLD,
			#endif

			#ifdef SIGCONT
			SIGCONT,
			#endif

			SIGFPE,

			#ifdef SIGHUP
			SIGHUP,
			#endif

			SIGILL,
			SIGINT,

			#ifdef SIGKILL
			SIGKILL,
			#endif

			#ifdef SIGPIPE
			SIGPIPE,
			#endif

			#ifdef SIGQUIT
			SIGQUIT,
			#endif

			#ifdef SIGSTOP
			SIGSTOP,
			#endif

			SIGSEGV,
			SIGTERM,

			#ifdef SIGTSTP
			SIGTSTP,
			#endif

			#ifdef SIGTIN
			SIGTIN,
			#endif

			#ifdef SIGTOU
			SIGTOU,
			#endif

			#ifdef SIGUSR1
			SIGUSR1,
			#endif

			#ifdef SIGUSR2
			SIGUSR2,
			#endif

			#ifdef SIGPOLL
			SIGPOLL,
			#endif

			#ifdef SIGPROF
			SIGPROF,
			#endif

			#ifdef SIGPWR
			SIGPWR,
			#endif

			#ifdef SIGSTKFLT
			SIGSTKFLT,
			#endif

			#ifdef SIGSYS
			SIGSYS,
			#endif

			#ifdef SIGTRAP
			SIGTRAP,
			#endif

			#ifdef SIGURG
			SIGURG,
			#endif

			#ifdef SIGVTALRM
			SIGVTALRM,
			#endif

			#ifdef SIGWINCH
			SIGWINCH,
			#endif

			#ifdef SIGXCPU
			SIGXCPU,
			#endif

			#ifdef SIGXFSZ
			SIGXFSZ,
			#endif
		};

		return buf;
	}

	auto sigmap()
	{
		struct
		{
			std::map<sys::sig::string, int> map;
			std::map<int, sys::sig::string> inv;

		} sigs;

		auto buf = sigset();
		for (auto n : buf)
		{
			auto u = sigstr(n);
			sigs.map[u] = n;
			sigs.inv[n] = u;
		}
		return sigs;
	}

	const auto sigs = sigmap();

	sys::exclusive<std::map<sys::sig::string, sys::sig::function>> socket;
}


namespace sys::sig
{
	void raise(string key)
	{
		if (auto it = sigs.find(key); sigs.end() != it)
		{
			std::raise(it->second);
		}
		else
		{
			auto reader = socket.reader();
			auto it = reader->find(key);
			if (reader->end() != it)
			{
				auto call = it->second;
				sys::thread job = [call, key]()
				{
					std::invoke(call, key);
				};
			}
		}
	}

	void notify(string key, function call)
	{
		auto writer = socket.writer();
		auto it = writer->find(key);
		if (writer->end() != it)
		{
			auto old = it->second;
			*it = [call, old](string key)
			{
				std::invoke(old, key);
				std::invoke(call, key);
			});
		}
		else
		{
			writer->emplace(key, call);
		}
	}
}
