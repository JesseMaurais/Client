#ifndef uni_notify
#define uni_notify "Notify Events"

#include "err.hpp"
#include "ptr.hpp"
#include "sys.hpp"
#include "mode.hpp"

#include <sys/inotify.h>
#include <linux/limits.h>

namespace sys::linux::inotify
{
	struct events
	{
		static constexpr size_t size = sizeof(inotify_event) + NAME_MAX + 1;

		events(int fd) : off(0)
		{
			for (ssize_t n = 0, sz = size; 0 < sz; n += sz)
			{
				buf.resize(n + sz);
				sz = sys::read(fd, buf.data() + n, buf.size() - n);
				if (sys::fail(sz) and sys::err(here, "read"))
				{
					buf.clear();
					break;
				}
				buf.resize(n + sz);
			}
		}

		auto value() const
		{
			auto const ptr = buf.data() + off;
			return fwd::cast_as<inotify_event>(ptr);
		}

		void next()
		{
			auto ev = value();
			off += sizeof(inotify_event);
			off += ev->len;

			if (buf.size() <= off)
			{
				buf.clear();
				off = 0;
			}
		}

	private:

		fmt::string buf;
		size_t off;
	};
}

#endif // file