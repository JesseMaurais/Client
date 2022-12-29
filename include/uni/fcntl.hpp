#ifndef fcntl_hpp
#define fcntl_hpp "POSIX File Control"

#include "uni.hpp"
#include "fmt.hpp"
#include "ptr.hpp"
#include <fcntl.h>
#include <aio.h>

namespace sys::uni
{
	struct lock : fwd::zero<flock>
	{
		auto get(int fd)
		{
			return fcntl(fd, F_GETLK, this);
		}

		auto set(int fd)
		{
			return fcntl(fd, F_SETLK, this);
		}

		auto wait(int fd)
		{
			return fcntl(fd, F_SETLKW, this);
		}
	};

	struct aio : fwd::zero<aiocb>
	{
		auto read()
		{
			return aio_read(this);
		}

		auto write()
		{
			return aio_write(this);
		}

		auto fsync(int op)
		{
			return aio_fsync(op, this);
		}

		auto error() const
		{
			return aio_error(this);
		}

		auto result()
		{
			return aio_return(this);
		}

		auto suspend(auto to=nullptr)
		{
			return aio_suspend(this, 1, to);
		}

		auto cancel(int fd)
		{
			return aio_cancel(fd, this);
		}
	};
}

#if __has_include(<sys/inotify.h>)
#define SYS_INOTIFY
#include <sys/inotify.h>
namespace sys::uni
{
	struct inotify
	{
		sys::uni::filed fd;

		inotify(int flags=0) : fd(inotify_init1(flags))
		{
			if (sys::fail(fd))
			{
				perror("inotify_init");
			}
		}

		auto add(const char* path, uint32_t mask)
		{
			int wd = inotify_add_watch(fd, path, mask);
			if (sys::fail(wd))
			{
				perror("inotify_add_watch");
			}
			return wd;
		}

		auto rm(int wd)
		{
			int no = inotify_rm_watch(fd, wd);
			if (sys::fail(no))
			{
				perror("inotify_rm_watch");
			}
			return no;
		}

		auto read(inotify_event* buf, size_t sz)
		{
			 ssize_t n = sys::read(fd, buf, sz);
			 if (sys::fail(n))
			 {
				perror("inotify_event");
			 }
			 return n;
		}
	};
}
#endif

#if __has_include(<sys/event.h>)
#define SYS_EVENT
#include <sys/event.h>
namespace sys::uni
{
	struct event : fwd::zero<kevent>
	{
		using zero::zero;
		event(uintptr_t id, auto fi=0, auto fl=0,  auto ffl=0, auto d=0, auto p=nullptr)
		{
			EV_SET(this, id, fi, fl, ffl, d, p);
		}
	};

	struct queue : fmt::layout<event>
	{
		using events = vector;
		using view = span;

		sys::uni::filed fd;

		queue() : fd(kqueue())
		{
			if (fail(fd)) perror("kqueue");
		}

		ssize_t operator()(view in, view out, const timespec* to=nullptr) const
		{
			int nev = kevent(fd, in.data(), in.size(), out.data(), out.size(), to);
			if (sys::fail(nev))
			{
				perror("kevent");
			}
			return nev
		}
	};
}
#endif

#endif // file