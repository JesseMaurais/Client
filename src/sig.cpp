// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "sig.hpp"
#include "fmt.hpp"
#include "sync.hpp"
#include <unordered_map>
#include <functional>
#include <csignal>

namespace
{
	static sys::exclusive<std::unordered_map<std::string_view, fwd::event>> sigmap;
	static sys::exclusive<std::unordered_map<int, fwd::event>> sighandler;
	static const std::unordered_map<std::string_view, int> sigint
	{
		#ifdef SIGABRT
		{ "abort", SIGABRT },
		#endif
		
		#ifdef SIGALRM
		{ "alarm", SIGALRM },
		#endif
		
		#ifdef SIGBUS
		{ "bus", SIGBUS },
		#endif
		
		#ifdef SIGCHLD
		{ "child", SIGCHLD },
		#endif
		
		#ifdef SIGCONT
		{ "continue", SIGCONT },
		#endif
		
		#ifdef SIGFPE
		{ "float", SIGFPE },
		#endif
		
		#ifdef SIGHUP
		{ "hangup", SIGHUP },
		#endif
		
		#ifdef SIGILL
		{ "illegal", SIGILL },
		#endif
		
		#ifdef SIGINT
		{ "interrupt", SIGINT },
		#endif
		
		#ifdef SIGKILL
		{ "kill", SIGKILL },
		#endif
		
		#ifdef SIGPIPE
		{ "pipe", SIGPIPE },
		#endif
		
		#ifdef SIGPOLL
		{ "poll", SIGPOLL },
		#endif
		
		#ifdef SIGQUIT
		{ "quit", SIGQUIT },
		#endif
		
		#ifdef SIGSEGV
		{ "segment", SIGSEGV },
		#endif
		
		#ifdef SIGSTOP
		{ "stop", SIGSTOP },
		#endif
		
		#ifdef SIGSYS
		{ "system", SIGSYS },
		#endif
		
		#ifdef SIGTERM
		{ "terminate", SIGTERM },
		#endif
		
		#ifdef SIGTSTP
		{ "pause", SIGTSTP },
		#endif
		
		#ifdef SIGTTIN
		{ "input", SIGTTIN },
		#endif
		
		#ifdef SIGTTOU
		{ "output", SIGTTOU },
		#endif
		
		#ifdef SIGTRAP
		{ "trap", SIGTRAP },
		#endif
		
		#ifdef SIGURG
		{ "urgent", SIGURG },
		#endif
		
		#ifdef SIGUSR1
		{ "user1", SIGUSR1 },
		#endif
		
		#ifdef SIGUSR2
		{ "user2", SIGUSR2 },
		#endif
		
		#ifdef SIGXCPU
		{ "time", SIGXCPU },
		#endif
		
		#ifdef SIGXFSZ
		{ "size", SIGXFSZ },
		#endif
	};
	
	static void handler(int no)
	{
		auto reader = sighandler.reader();
		if (auto it = reader->find(no); reader->end() != it)
		{
			#ifdef assert
			assert(no == it->first);
			#endif
			std::invoke(it->second);
		}
	}
}

namespace sys::sig
{
	void set(fmt::view key, fwd::event value)
	{
		if (auto it = sigint.find(key); sigint.end() != it)
		{
			#ifdef assert
			assert(key == it->first);
			#endif

			auto writer = sighandler.writer();
			writer->emplace(it->second, value);

			auto f = value ? handler : SIG_DFL;
			if (SIG_ERR == std::signal(it->second, f))
			{
				#ifdef perror
				perror("signal", key);
				#endif
			}
		}
		auto writer = sigmap.writer();
		writer->emplace(key, value);
	}
	
	fwd::event get(fmt::view key)
	{
		auto reader = sigmap.reader();
		if (auto it = reader->find(key); reader->end() != it)
		{
			#ifdef assert
			assert(key == it->first);
			#endif
			return it->second;
		}
		return fwd::event();
	}
	
	void raise(fmt::view key)
	{
		if (auto handler = get(key); handler) 
		{
			std::invoke(handler);
		}
		else
		{
			#ifdef WARN
			WARN("Key not found:", key);
			#endif
		}
	}
}
