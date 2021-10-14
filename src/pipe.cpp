// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "pipe.hpp"
#include "type.hpp"
#include "dig.hpp"
#include "opt.hpp"
#include "sys.hpp"
#include "net.hpp"
#include "err.hpp"
#ifdef _WIN32
#include "win/file.hpp"
#include "win/memory.hpp"
#else
#include "uni.hpp"
#endif

namespace env::file
{
	ssize_t descriptor::write(const void* buf, size_t sz) const
	{
		auto const n = sys::write(fd, buf, sz);
		if (fail(n))
		{
			sys::err(here, fd, sz);
		}
		return n;
	}

	ssize_t descriptor::read(void* buf, size_t sz) const
	{
		auto const n = sys::read(fd, buf, sz);
		if (fail(n))
		{
			sys::err(here, fd, sz);
		}
		return n;
	}

	bool descriptor::open(fmt::string::view path, mode am, permit pm)
	{
		if (not fmt::terminated(path))
		{
			auto const s = fmt::to_string(path);
			return open(s, am, pm);
		}
		auto const c = path.data();

		fd = sys::open(c, convert(am), convert(pm));
		if (fail(fd))
		{
			sys::err(here, path, am, pm);
			return failure;
		}
		return success;
	}

	bool descriptor::close()
	{
		if (fail(sys::close(fd)))
		{
			sys::err(here, fd);
			return failure;
		}
		fd = invalid;
		return success;
	}

	dup::dup(int from, int to) : tempfd(to)
	{
		fd = sys::dup(to);
		if (fail(fd))
		{
			sys::err(here, "dup", to);
		}
		else
		if (fail(sys::dup2(from, to)))
		{
			sys::err(here, "dup2", from, to);
			close();
		}
	}

	dup::~dup()
	{
		if (not fail(fd) and fail(sys::dup2(fd, tempfd)))
		{
			sys::err(here, "dup2", fd, tempfd);
		}
	}

	pipe::pipe()
	{
		int fd[2];
		if (fail(sys::pipe(fd)))
		{
			sys::err(here);
		}
		else set(fd);
	}

	bool process::start(fmt::string::view::init args)
	{
		fmt::string::view::vector t(args);
		return start(fmt::string::view::span(t));
	}

	bool process::start(fmt::string::view::span args)
	{
		fmt::string::view const del("\0", 1);
		std::vector<char const *> list;
		auto s = fmt::join(args, del);
		for (auto u : fmt::split(s, del))
		{
			list.push_back(data(u));
		}

		auto const argc = list.size();
		list.push_back(nullptr);
		auto const argv = list.data();

		return start(argc, argv);
	}

	bool process::start(size_t argc, char const **argv)
	{
		int fds[3];
		pid = sys::exec(fds, argc, argv);
		if (not fail(pid))
		{
			for (auto n : { 0, 1, 2 })
			{
				(void) fd[n].set(fds[n]);
			}
		}
		return fail(pid);
	}

	bool process::quit()
	{
		return fail(sys::kill(pid));
	}

	int process::wait()
	{
		return sys::wait(pid);
	}

	fifo::fifo(fmt::string::view name, mode mask)
	: flags(convert(mask))
	{
		#ifdef _WIN32
		{
			path = fmt::join({ "\\\\.\\pipe\\", name });

			size_t const size = width();
			sys::win::handle h = CreateNamedPipe
			(
				path.c_str(),
				PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
				PIPE_UNLIMITED_INSTANCES,
				size,   // output
				size,   // input
				0,      // default time out
				nullptr // default security
			);

			if (sys::win::fail(h))
			{
				sys::win::err(here, "CreateNamedPipe", path);
				path.clear();
				return;
			}

			fd = h.open(flags);
		}
		#else
		{
			auto const dir = fmt::dir::join({env::usr::run_dir(), ".fifo"});
			auto const s = dir.data();

			constexpr mode_t rw = S_IRGRP | S_IWGRP;
			sys::mode const um;

			if (fail(sys::access(s, F_OK)))
			{
				if (fail(sys::mkdir(s, um | rw)))
				{
					sys::err(here, "mkdir", dir);
					return;
				}
			}

			if (fail(sys::access(s, R_OK | W_OK)))
			{
				if (fail(sys::chmod(s, um | rw)))
				{
					sys::err(here, "chmod", dir);
					return;
				}
			}

			path = fmt::dir::join({dir, name});
			auto const c = path.data();
			if (fail(mkfifo(c, um | rw)))
			{
				sys::err(here, "mkfifo", path);
				path.clear();
			}
		}
		#endif
	}

	bool fifo::connect()
	{
		#ifdef _WIN32
		{
			auto const h = sys::win::get(fd);
			bool const ok = not sys::win::fail(h);
			if (ok and not ConnectNamedPipe(h, nullptr))
			{
				sys::win::err(here, "ConnectNamedPipe");
				path.clear();
				return false;
			}
			return ok;
		}
		#else
		{
			sys::mode const um;
			auto const s = data(path);
			fd = sys::open(s, flags, (mode_t) um);
			if (fail(fd))
			{
				sys::err(here, "open", path);
				path.clear();
				return false;
			}
			return true;
		}
		#endif
	}

	fifo::~fifo()
	{
		#ifdef _WIN32
		{
			auto const h = sys::win::get(fd);
			bool const ok = not sys::win::fail(h);
			if (ok and not DisconnectNamedPipe(h))
			{
				sys::win::err(here, "DisconnectNamedPipe");
			}
		}
		#else
		{
			if (not empty(path))
			{
				auto const c = path.data();
				if (fail(sys::unlink(c)))
				{
					sys::err(here, "unlink", path);
				}
			}
		}
		#endif
	}

	socket::socket(int fd)
	{
		this->fd = fd;
	}

	socket::socket(int family, int type, int proto)
	{
		#ifdef _WIN32
		{
			static auto const param = fmt::put("WSA");
			static long const value = MAKEWORD(2, 2);
			static auto const ver = env::opt::get(param, value);
			static sys::net::win::data ws(fmt::to<WORD>(ver));
		}
		#endif

		fd = sys::net::socket(family, type, proto);
		if (sys::net::fail(fd))
		{
			sys::net::err(here, "socket");
		}
	}

	socket::~socket()
	{
		bool const ok = not sys::net::fail(fd);
		if (ok and sys::net::close(fd))
		{
			sys::net::err(here, "closesocket");
		}
	}

	socket::operator bool() const
	{
		return not sys::net::fail(fd);
	}

	socket socket::accept(address& name, size_t* length) const
	{
		sys::net::size n;
		auto const so = sys::net::accept(fd, &name.address, &n);
		if (sys::net::fail(so))
		{
			sys::net::err(here, "accept");
		}
		else
		if (nullptr != length)
		{
			*length = fmt::to_size(n);
		}
		return so;
	}

	bool socket::connect(address const& name, size_t length) const
	{
		if (sys::net::connect(fd, &name.address, length))
		{
			sys::net::err(here, "connect");
			return false;
		}
		return true;
	}

	bool socket::bind(address const& name, size_t length) const
	{
		if (sys::net::bind(fd, &name.address, length))
		{
			sys::net::err(here, "bind");
			return false;
		}
		return true;
	}

	bool socket::listen(int backlog) const
	{
		if (sys::net::listen(fd, backlog))
		{
			sys::net::err(here, "listen");
			return false;
		}
		return true;
	}

	bool socket::shutdown(int how) const
	{
		if (sys::net::shutdown(fd, how))
		{
			sys::net::err(here, "shutdown");
			return false;
		}
		return true;
	}

	ssize_t socket::write(const void *data, size_t size, int flags) const
	{
		auto ptr = static_cast<sys::net::const_pointer>(data);
		ssize_t const n = sys::net::send(fd, ptr, size, flags);
		if (n < 0)
		{
			sys::net::err(here, "send");
		}
		return n;
	}

	ssize_t socket::write(const void *data, size_t size, int flags, address const& name, size_t length) const
	{
		auto ptr = static_cast<sys::net::const_pointer>(data);
		ssize_t const n = sys::net::sendto(fd, ptr, size, flags, &name.address, length);
		if (n < 0)
		{
			sys::net::err(here, "sendto");
		}
		return n;
	}

	ssize_t socket::read(void *data, size_t size, int flags) const
	{
		auto ptr = static_cast<sys::net::pointer>(data);
		ssize_t const n = sys::net::recv(fd, ptr, size, flags);
		if (n < 0)
		{
			sys::net::err(here, "recv");
		}
		return n;
	}

	ssize_t socket::read(void *data, size_t size, int flags, address& name, size_t& length) const
	{
		sys::net::size m = length;
		auto ptr = static_cast<sys::net::pointer>(data);
		ssize_t const n = sys::net::recvfrom(fd, ptr, size, flags, &name.address, &m);
		if (n < 0)
		{
			sys::net::err(here, "recvfrom");
		}
		else length = fmt::to_size(m);
		return n;
	}

	fwd::extern_ptr<void> make_map(int fd, size_t sz, off_t off, mode am, size_t *out)
	{
		assert(not fail(fd));
		assert(am & rwx);

		if (nullptr == out) out = &sz;

		#ifdef _WIN32
		{
			DWORD flags = 0;
			DWORD prot = 0;

			if (am & ex)
			{
				flags |= FILE_MAP_EXECUTE;

				if (am & wr)
				{
					prot = PAGE_EXECUTE_READWRITE;
					flags |= FILE_MAP_WRITE;
				}
				else
				if (am & rd)
				{
					prot = PAGE_EXECUTE_READ;
					flags |= FILE_MAP_READ;
				}
			}
			else
			{
				if (am & wr)
				{
					prot = PAGE_READWRITE;
					flags |= FILE_MAP_WRITE;
				}
				else
				if (am & rd)
				{
					prot = PAGE_READONLY;
					flags |= FILE_MAP_READ;
				}
			}

			sys::win::handle const h = CreateFileMapping
			(
				sys::win::get(fd),
				nullptr,
				prot,
				HIWORD(sz),
				LOWORD(sz),
				nullptr
			);

			if (sys::win::fail(h))
			{
				sys::win::err(here, "CreateFileMapping");
			}

			sys::win::file_info info(h);
			*out = info.nFileSizeHigh;
			*out <<= CHAR_BIT * sizeof info.nFileSizeHigh;
			*out |= info.nFileSizeLow;

			return sys::win::mem::map(h, flags, off, sz);
		}
		#else
		{
			if (0 == sz)
			{
				struct sys::stat st(fd);
				if (sys::fail(st))
				{
					sys::err(here, "stat");
				}
				else sz = st.st_size;
			}
			*out = sz;

			int prot = 0;
			if (am & rd) prot |= PROT_READ;
			if (am & wr) prot |= PROT_WRITE;
			if (am & ex) prot |= PROT_EXEC;

			return sys::uni::shm::map(sz, prot, MAP_PRIVATE, fd, off);
		}
		#endif
	}
}

