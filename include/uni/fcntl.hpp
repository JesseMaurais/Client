#ifndef fcntl_hpp
#define fcntl_hpp "POSIX File Control"

#include "uni.hpp"
#include "fmt.hpp"
#include "ptr.hpp"
#include <fcntl.h>

namespace sys::uni::file
{
	struct lock : fwd::zero<flock>
	{
		bool get(int fd)
		{
			return fail(fcntl(fd, F_GETLK, this));
		}

		bool set(int fd)
		{
			return fail(fcntl(fd, F_SETLK, this));
		}

		bool wait(int fd)
		{
			return fail(fcntl(fd, F_SETLKW, this));
		}
	};
}

#if __has_include(<sys/inotify.h>)
#include <sys/inotify.h>
#define SYS_INOTIFY_H
namespace sys::kernel
{
	struct inotify
	{
		sys::uni::filed wd;

		watch(int flags=0) : wd(inotify_init1(flags))
		{
			if (sys::fail(wd))
			{
				sys::err(here, "inotify_init", flags);
			}
		}

		auto add(const char* path, uint32_t mask)
		{
			int fd = inotify_add_watch(wd, path, mask);
			if (sys::fail(fd))
			{
				sys::err(here, "inotify_add_watch", path, mask);
			}
			return fd;
		}

		auto rm(int fd)
		{
			int no = inotify_rm_watch(fd, wd);
			if (sys::fail(no))
			{
				sys::err(here, "inotify_rm_watch", fd);
			}
			return no;
		}

		auto read(inotify_event* buf, size_t sz)
		{
			 ssize_t ssz = sys::read(wd, buf, sz);
			 if (sys::fail(ssz))
			 {
				sys::err(here, "inotify_event");
			 }
			 return ssz;
		}
	};
}
#endif

#if __has_include(<sys/event.h>)
#include <sys/event.h>
#define SYS_EVENT_H
namespace sys::kernel
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
			#ifdef warn
			warn(fail(fd));
			#endif
		}

		ssize_t operator()(view in, view out, const timespec* to=nullptr) const
		{
			int nev = kevent(fd, in.data(), in.size(), out.data(), out.size(), to);
			if (sys::fail(nev))
			{
				sys::err(here, "kevent");
			}
			return nev
		}
	};
}
#endif


#endif // file