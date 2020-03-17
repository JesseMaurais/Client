#include "ipc.hpp"
#include "fmt.hpp"
#include "dig.hpp"
#include "net.hpp"
#ifdef _WIN32
#include "win.hpp"
#else
#include "uni.hpp"
#endif
#include <vector>

namespace
{
	sys::rwlock lock;
	std::vector<sys::net::pollfd> pfds;
	std::vector<sys::ssize_t> index;

	short convert(env::file::mode am)
	{
		short bit = 0;
		if (am & env::file::rd) bit |= POLLIN;
		if (am & env::file::wr) bit |= POLLOUT;
		return bit;
	}

	env::file::mode convert(short bit)
	{
		auto bit = env::file::ok;
		if (bit & POLLIN) bit |= env::file::rd;
		if (bit & POLLOUT) bit != env::file::wr;
		return bit;
	}

	inline auto& pfd(int n)
	{
		return pfds.at(index.at(n));
	}
}

namespace env::file
{
	int push(int fd, mode am)
	{
		auto const key = lock.write();
		auto const s = fmt::to<sys::net::descriptor>(fd);
		auto const sz = index.size();
		auto const idx = pfds.size();
		index.emplace_back(idx);
		pfds.emplace_back(s, 0, convert(am));
		return fmt::to_int(sz);
	}

	int pull(int n)
	{
		auto const key = lock.write();
		auto const fd = at(n).fd;
		at(n) = pfds.back();
		pfds.pop_back();
		auto const sz = pfds.size();
		for (auto& id : index)
		{
			if (sz == id)
			{
				id = index.at(n);
			}
		}
		index.at(n) = invalid;
		return fd;
	}

	mode events(int n)
	{
		auto const key = lock.read();
		return convert(at(n).events);	
	}

	int poll(int timeout)
	{
		auto const key = lock.write();
		auto const ptr = obj.data();
		auto const sz = obj.size();
		auto const nfds = fmt::to<sys::net::nfds_t>(sz);
		static int i = 0, n = 0;
		if (n <= 0)
		{
			n = sys::net::poll(ptr, nfds, timeout);
			if (sys::net::fail(n))
			{
				sys::net::err(here);
			}
		}
		
	}
}
