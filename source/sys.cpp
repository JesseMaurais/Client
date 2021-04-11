// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "sys.hpp"
#include "err.hpp"
#include "sym.hpp"
#include "sig.hpp"
#include "ptr.hpp"
#include "dir.hpp"
#include "env.hpp"
#include "pipe.hpp"
#include "sync.hpp"
#include "type.hpp"
#ifdef _WIN32
#include "win/message.hpp"
#else
#include "uni/signal.hpp"
#include <dlfcn.h>
#endif

namespace sys
{
	bool debug =
	#ifdef NDEBUG
		false;
	#else
		true;
	#endif

	thread_local fmt::string::view thread_id;
	thread_local fmt::string::stream thread_buf;

	fmt::string::out::ref out()
	{
		return thread_buf;
	}

	fmt::string::out::ref put(fmt::string::out::ref buf)
	{
		static sys::mutex key;
		auto const unlock = key.lock();

		fmt::string line;
		while (std::getline(thread_buf, line))
		{
			buf << line << std::endl;
		}
		return buf;
	}

	int impl::bug(fmt::string::view message, bool no)
	{
		thread_local struct
		{
			fmt::string last;
			int counter = -1;
		} local;
		// Avoid spamming
		if (message != local.last)
		{
			// reset
			local.counter = 0;
			local.last = fmt::to_string(message);
			// format
			{
				// message
				thread_buf << local.last;
				// number
				if (no)
				{
					thread_buf << ':' << ' ' << std::strerror(errno);
				}
				// thread
				if (not empty(thread_id))
				{
					thread_buf << ' ' << '[' << thread_id << ']';
				}
			}
			thread_buf << fmt::eol;
		}
		else ++local.counter;
		return local.counter;
	}

	#ifdef _WIN32
	namespace win
	{
		char const* strerr(unsigned long id, void* ptr)
		{
			constexpr auto flag = FORMAT_MESSAGE_ALLOCATE_BUFFER
			                    | FORMAT_MESSAGE_IGNORE_INSERTS
			                    | FORMAT_MESSAGE_FROM_HMODULE
			                    | FORMAT_MESSAGE_FROM_SYSTEM;

			auto h = static_cast<HANDLE>(ptr);
			if (sys::win::fail(h))
			{
				h = GetModuleHandle(nullptr);
			}

			thread_local auto tls = fwd::null_ptr<HLOCAL>(LocalFree);

			LPSTR str = nullptr;
			auto addr = (LPSTR) &str;
			auto const size = FormatMessage
			(
				flag,   // style
				h,      // module
				id,     // message
				0,      // language
				addr,   // buffer
				0,      // size
				nullptr // arguments
			);

			if (0 < size) // replace
			{
				tls.reset(reinterpret_cast<HLOCAL>(str));
			}

			if (nullptr == str)
			{
				thread_local char buf[128] = { '\0' };
				std::sprintf(buf, "Unknown error %lu", id);
				str = buf;
			}

			return str;
		}
	}
	#else
	namespace uni
	{
		char const* strerr(int no)
		{
			thread_local char buf[128] = { '\0' };
			// POSIX and GNU differ on return type
			(void) strerror_r(no, buf, sizeof buf);
			return buf;
		}
	}
	#endif

	pid_t exec(int fd[3], size_t argc, char const **argv)
	{
		assert(nullptr != argv);
		assert(nullptr != argv[0]);
		assert(nullptr == argv[argc]);

		#ifdef _WIN32
		{
			sys::win::pipe pair[3];

			for (int n : { 0, 1, 2 })
			{
				if (fail(pair[n].read))
				{
					return -1;
				}

				auto const h = n ? pair[n].read.h : pair[n].write.h;

				if (not SetHandleInformation(h, HANDLE_FLAG_INHERIT, false))
				{
					sys::win::err(here, "SetHandleInformation");
					return sys::invalid;
				}
			}

			char cmd[MAX_PATH];
			ZeroMemory(cmd, sizeof cmd);
			for (size_t i = 0, j = 0; argv[i]; ++i, ++j)
			{
				constexpr auto max = sizeof cmd;
				bool const any_space = fmt::any_of(argv[i], fmt::space);
				auto const format = any_space ? "\"%s\" " : "%s ";
				auto const n = std::snprintf(cmd + j, max - j, format, argv[i]);
				if (0 < n) j += n - 1;
				else return -1;
			}

			sys::win::zero<PROCESS_INFORMATION> pi;
			sys::win::size<&STARTUPINFO::cb> si;

			si.dwFlags = STARTF_USESTDHANDLES;
			si.hStdInput = pair[0].read.h;
			si.hStdOutput = pair[1].write.h;
			si.hStdError = pair[2].write.h;

			bool const ok = CreateProcess
			(
			 nullptr,          // application
			 cmd,              // command line
			 nullptr,          // process attributes
			 nullptr,          // thread attributes
			 true,             // inherit handles
			 DETACHED_PROCESS, // creation flags
			 nullptr,          // environment
			 nullptr,          // current directory
			 &si,              // start-up info
			 &pi               // process info
			);

			if (not ok)
			{
				sys::win::err(here, "CreateProcess", cmd);
				return sys::invalid;
			}

			sys::win::handle const closed(pi.hThread);

			for (int n : { 0, 1, 2 })
			{
				fd[n] = n ? pair[n].read.open(O_RDONLY) : pair[n].write.open(O_WRONLY);
			}

			return pi.dwProcessId;
		}
		#else
		{
			env::file::pipe pair[3];

			for (auto const& p : pair)
			{
				for (int n : { 0, 1, 2 })
				{
					int fd = p[n].get();
					if (fail(fd))
					{
						return sys::invalid;
					}
				}
			}

			pid_t const pid = fork();
			if (pid)
			{
				if (fail(pid))
				{
					sys::err(here, "fork");
				}
				else for (int i : { 0, 1, 2 })
				{
					fd[i] = pair[i][0 == i].set();
				}
				return pid;
			}

			for (int i : { 0, 1, 2 })
			{
				int k = pair[i][0 != i].get();

				if (fail(close(i)) or fail(dup2(k, i)))
				{
					exit(EXIT_FAILURE);
				}

				for (int j : { 0, 1 })
				{
					k = pair[i][j].set();

					if (fail(close(k)))
					{
						exit(EXIT_FAILURE);
					}
				}
			}

			std::vector<char*> args;
			for (int i = 0; argv[i]; ++i)
			{
				args.push_back(const_cast<char*>(argv[i]));
			}
			args.push_back(nullptr);

			int const res = execvp(args.front(), args.data());
			sys::err(here, "execvp", res, args.front());
			std::exit(res);
		}
		#endif
	}

	bool kill(pid_t pid)
	{
		#ifdef _WIN32
		{
			constexpr auto dw = PROCESS_ALL_ACCESS;
			sys::win::handle const h = OpenProcess(dw, false, pid);
			if (fail(h))
			{
				sys::win::err(here, "OpenProcess", pid);
				return failure;
			}
			else
			if (not TerminateProcess(h, 0))
			{
				sys::win::err(here, "TerminateProcess", pid);
				return failure;
			}
			return success;
		}
		#else
		{
			if (fail(::kill(pid, SIGTERM)))
			{
				sys::err(here, "SIGTERM", pid);
				return failure;
			}
			return success;
		}
		#endif
	}

	int wait(pid_t pid)
	{
		#ifdef _WIN32
		{
			auto code = static_cast<DWORD>(sys::invalid);
			constexpr auto dw = PROCESS_ALL_ACCESS;
			sys::win::handle const h = OpenProcess(dw, false, pid);
			if (fail(h))
			{
				sys::win::err(here, "OpenProcess", pid);
			}
			else
			{
				if (sys::win::wait(h))
				{
					if (not GetExitCodeProcess(h, &code))
					{
						sys::win::err(here, "GetExitCodeProcess", pid);
					}
				}
			}
			return static_cast<int>(code);
		}
		#else
		{
			int status = -1;
			pid_t const parent = pid;
			do
			{
				pid = waitpid(parent, &status, 0);
				if (fail(pid))
				{
					sys::err(here, "waitpid", parent);
				}
			}
			while (pid != parent);

			if (WIFEXITED(status))
			{
				return WEXITSTATUS(status);
			}
			else
			if (WIFSTOPPED(status))
			{
				return WSTOPSIG(status);
			}
			else
			if (WIFSIGNALED(status))
			{
				return WTERMSIG(status);
			}

			return status; // unreachable?
		}
		#endif
	}

	// sym.hpp

	dll & bin()
	{
		static dll local;
		return local;
	}

	dll::dll(fmt::string::view path)
	{
		auto const buf = fmt::to_string(path);
		auto const s = buf.data();

		#ifdef _WIN32
		{
			auto const h = LoadLibrary(s);
			if (nullptr == h)
			{
				sys::win::err(here, "LoadLibrary", path);
			}
			else ptr = h;
		}
		#else
		{
			ptr = dlopen(s, RTLD_LAZY);
			if (nullptr == ptr)
			{
				auto const e = dlerror();
				sys::warn(here, "dlopen", path, e);
			}
		}
		#endif
	}

	dll::~dll()
	{
		#ifdef _WIN32
		{
			auto const h = static_cast<HMODULE>(ptr);
			if (nullptr != h and not FreeLibrary(h))
			{
				sys::win::err(here, "FreeLibrary");
			}
		}
		#else
		{
			if (nullptr != ptr and dlclose(ptr))
			{
				auto const e = dlerror();
				sys::warn(here, "dlclose", e);
			}
		}
		#endif
	}

	void *dll::sym(fmt::string::view name) const
	{
		auto const buf = fmt::to_string(name);
		auto const s = buf.data();

		#ifdef _WIN32
		{
			auto h = static_cast<HMODULE>(ptr);
			if (nullptr == h)
			{
				h = GetModuleHandle(nullptr);
				if (nullptr == h)
				{
					sys::win::err(here, "GetModuleHandle");
					return nullptr;
				}
			}

			auto const f = GetProcAddress(h, s);
			if (nullptr == f)
			{
				sys::win::err(here, "GetProcAddress", name);
			}
			return f;
		}
		#else
		{
			(void) dlerror();
			auto f = dlsym(ptr, s);
			auto const e = dlerror();
			if (nullptr != e)
			{
				sys::warn(here, "dlsym", name, e);
			}
			return f;
		}
		#endif
	}

	dll dll::find(fmt::string::view basename)
	{
		using namespace env::file;
		fmt::string name = fmt::to_string(basename) + sys::ext::share;
		env::file::find(env::paths(), regx(name) || to(name) || stop);
		return fmt::string::view(name);
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

	fmt::string to_string(int signo)
	{
		switch (signo)
		{
		case SIGABRT:
			return "Abort";

		#ifdef SIGALRM
		case SIGALRM:
			return "Alarm";
		#endif

		#ifdef SIGBUS
		case SIGBUS:
			return "Memory access violation";
		#endif

		#ifdef SIGCHLD
		case SIGCHLD:
			return "Child process stopped/continued";
		#endif

		#ifdef SIGCONT
		case SIGCONT:
			return "Continue processing";
		#endif

		case SIGFPE:
			return "Float-point error";

		#ifdef SIGHUP
		case SIGHUP:
			return "Hang up";
		#endif

		case SIGILL:
			return "Illegal instruction";

		case SIGINT:
			return "Interrupt";

		#ifdef SIGKILL
		case SIGKILL:
			return "Kill";
		#endif

		#ifdef SIGPIPE
		case SIGPIPE:
			return "Broken pipe";
		#endif

		#ifdef SIGQUIT
		case SIGQUIT:
			return "Quit processing";
		#endif

		case SIGSEGV:
			return "Segmentation fault";

		#ifdef SIGSTOP
		case SIGSTOP:
			return "Stop processing";
		#endif

		case SIGTERM:
			return "Terminate";

		#ifdef SIGTSTP
		case SIGTSTP:
			return "Terminal stopped";
		#endif

		#ifdef SIGTIN
		case SIGTIN:
			return "Process reading";
		#endif

		#ifdef SIGTOU
		case SIGTOU:
			return "Process writing";
		#endif

		#ifdef SIGUSR1
		case SIGUSR1:
			return "User defined (1)";
		#endif

		#ifdef SIGUSR2
		case SIGUSR2:
			return "User defined (2)";
		#endif

		#ifdef SIGPOLL
		case SIGPOLL:
			return "Poll event";
		#endif

		#ifdef SIGPROF
		case SIGPROF:
			return "Profiling timer expired";
		#endif

		#ifdef SIGPWR
		case SIGPWR:
			return "Power limited";
		#endif

		#ifdef SIGSTKFLT
		case SIGSTKFLT:
			return "Stack fault";
		#endif

		#ifdef SIGSYS
		case SIGSYS:
			return "Bad system call";
		#endif

		#ifdef SIGTRAP
		case SIGTRAP:
			return "Trace or breakpoint trap";
		#endif

		#ifdef SIGURG
		case SIGURG:
			return "Urgent out of band data";
		#endif

		#ifdef SIGVTALRM
		case SIGVTALRM:
			return "Virtual timer expired";
		#endif

		#ifdef SIGWINCH
		case SIGWINCH:
			return "Window resized";
		#endif

		#ifdef SIGXCPU
		case SIGXCPU:
			return "CPU time limit exceeded";
		#endif

		#ifdef SIGXFSZ
		case SIGXFSZ:
			return "File size limit exceeded";
		#endif

		default:
			return fmt::to_string(signo);
		}
	}
}

#ifdef test_unit

static int hidden() { return 42; }
dynamic int visible() { return hidden(); }

test_unit(sym)
{
	// Can see dynamic
	auto f = sys::sym<int()>("visible");
	assert(nullptr != f);
	// Cannot see static
//	auto g = sys::sym<int()>("hidden");
//	assert(nullptr == g);
	// Callable object
	assert(f() == hidden());
}

test_unit(sig)
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
