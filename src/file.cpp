// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "file.hpp"
#include "mode.hpp"
#include "pipe.hpp"
#include "fifo.hpp"
#include "socket.hpp"
#include "type.hpp"
#include "shm.hpp"
#include "fmt.hpp"
#include "dig.hpp"
#include "opt.hpp"
#include "arg.hpp"
#include "usr.hpp"
#include "dir.hpp"
#include "err.hpp"
#include "sys.hpp"
#include "sync.hpp"
#include "net.hpp"
#include <climits>
#include <utility>
#include <algorithm>
#include <regex>
#include <stack>

#ifdef _WIN32
# include "win/memory.hpp"
# include "win/file.hpp"
#else
# include "uni/dirent.hpp"
# include "uni/mman.hpp"
#endif

namespace fmt::dir
{
	string join(string::view::span p)
	{
		return fmt::join(p, sys::sep::dir);
	}

	string join(string::view::init p)
	{
		return fmt::join(p, sys::sep::dir);
	}

	string::view::vector split(string::view u)
	{
		return fmt::split(u, sys::sep::dir);
	}
}

namespace fmt::path
{
	string join(string::view::span p)
	{
		return fmt::join(p, sys::sep::path);
	}

	string join(string::view::init p)
	{
		return fmt::join(p, sys::sep::path);
	}

	string::view::vector split(string::view u)
	{
		return fmt::split(u, sys::sep::path);
	}
}

namespace env::file
{
	// file.hpp

	file_ptr make_ptr(fwd::as_ptr<FILE> fp)
	{
		return fwd::make_ptr(fp, [](auto fp)
		{
			if (std::fclose(fp))
			{
				sys::err(here, "fclose");
			}
		});
	}

	// mode.hpp

	fwd::variable<size_t>& width()
	{
		static sys::atomic<size_t> safe = BUFSIZ;
		return safe;
	}

	int convert(mode am)
	{
		int flags = 0;

		if (am & rw)
		{
			flags |= O_RDWR;
		}
		else
		if (am & wr)
		{
			flags |= O_WRONLY;
		}
		else
		if (am & rd)
		{
			flags |= O_RDONLY;
		}

		if (am & txt)
		{
			flags |= O_TEXT;
		}
		else
		if (am & bin)
		{
			flags |= O_BINARY;
		}

		if (am & app)
		{
			flags |= O_APPEND;
		}

		if (am & sz)
		{
			flags |= O_TRUNC;
		}

		if (am & xu)
		{
			flags |= O_EXCL;
		}

		if (am & ok)
		{
			flags |= O_CREAT;
		}

		return flags;
	}

	int convert(permit pm)
	{
		int flags = 0;

		if (pm & owner_x)
		{
			flags |= S_IXUSR;
		}
		if (pm & owner_w)
		{
			flags |= S_IWUSR;
		}
		if (pm & owner_r)
		{
			flags |= S_IRUSR;
		}
		if (pm & group_x)
		{
			flags |= S_IXGRP;
		}
		if (pm & group_w)
		{
			flags |= S_IWGRP;
		}
		if (pm & group_r)
		{
			flags |= S_IRGRP;
		}
		if (pm & other_x)
		{
			flags |= S_IXOTH;
		}
		if (pm & other_w)
		{
			flags |= S_IWOTH;
		}
		if (pm & other_r)
		{
			flags |= S_IROTH;
		}

		return flags;
	}

	bool fail(fmt::string::view path, mode am)
	{
		if (not fmt::terminated(path))
		{
			return fail(fmt::to_string(path), am);
		}
		auto const c = path.data();

		#ifdef _WIN32
		if (am & ex)
		{
			DWORD dw;
			return GetBinaryType(c, &dw)
				? success : failure;
		}
		#endif

		if (am == (am & rwx))
		{
			int flags = 0;

			if (am & ok)
			{
				flags |= F_OK;
			}
			if (am & ex)
			{
				flags |= X_OK;
			}
			if (am & rd)
			{
				flags |= R_OK;
			}
			if (am & wr)
			{
				flags |= W_OK;
			}

			return sys::fail(sys::access(c, flags));
		}

		struct sys::stat state(c);
		if (sys::fail(state))
		{
			return failure;
		}

		if ((am & dir) and not S_ISDIR(state.st_mode))
		{
			return failure;
		}
		if ((am & chr) and not S_ISCHR(state.st_mode))
		{
			return failure;
		}
		if ((am & reg) and not S_ISREG(state.st_mode))
		{
			return failure;
		}
		if (am & fifo)
		{
			#ifdef _WIN32
			if (not path.starts_with(R"(\.\pipe\)"))
			#endif
			#ifdef S_ISFIFO
			if (not S_ISFIFO(state.st_mode))
			#endif
				return failure;
		}
		if (am & sock)
		{
			#ifdef S_IFSOCK
			if (not S_ISSOCK(state.st_mode))
			#endif
				return failure;
		}
		if (am & blk)
		{
			#ifdef S_ISFBLK
			if (not S_ISFBLK(state.st_mode)
			#endif
				return failure;
		}
		if (am & reg)
		{
			#ifdef S_ISREG
			if (not S_ISREG(state.st_mode))
			#endif
				return failure;
		}
		if (am & lnk)
		{
			#ifdef S_ISLNK
			if (not S_ISLNK(state.st_mode))
			#endif
				return failure;
		}

		return success;
	}

	// dir.hpp

	fmt::string::view::edges paths()
	{
		return { env::pwd(), env::paths() };
	}

	fmt::string::view::edges config()
	{
		return { env::usr::config_home(), env::usr::config_dirs() };
	}

	fmt::string::view::edges data()
	{
		return { env::usr::data_home(), env::usr::data_dirs() };
	}

	bool find(fmt::string::view path, entry check)
	{
		if (not fmt::terminated(path))
		{
			return find(fmt::to_string(path), check);
		}
		auto const c = path.data();

		return fwd::any_of(sys::files(c), check);
	}

	bool find(fmt::string::view::span paths, entry check)
	{
		return fwd::any_of(paths, [check](auto path)
		{
			return find(path, check);
		});
	}

	bool find(fmt::string::view::edges paths, entry look)
	{
		return find(paths.first, look) or find(paths.second, look);
	}

	entry mask(mode am)
	{
		return [am](fmt::string::view u)
		{
			return not env::file::fail(u, am);
		};
	}

	entry regx(fmt::string::view u)
	{
		auto const s = fmt::to_string(u);
		auto const x = std::regex(s);
		return [x](fmt::string::view u)
		{
			std::cmatch cm;
			auto const s = fmt::to_string(u);
			return std::regex_search(s.data(), cm, x);
		};
	}

	entry to(fmt::string::vector& t)
	{
		return [&](fmt::string::view u)
		{
			auto s = fmt::to_string(u);
			t.emplace_back(move(s));
			return success;
		};
	}

	entry to(fmt::string& s)
	{
		return [&](fmt::string::view u)
		{
			s = fmt::to_string(u);
			return success;
		};
	}

	entry all(fmt::string::view u, mode m, entry e)
	{
		return mask(m) || regx(u) || e;
	}

	entry any(fmt::string::view u, mode m, entry e)
	{
		return all(u, m, e);
	}

	fmt::string::view make_dir(fmt::string::view path)
	{
		std::stack<fmt::string::view> stack;
		fmt::string buf;

		auto folders = fmt::dir::split(path);
		auto stem = path;

		while (env::file::fail(stem))
		{
			stack.push(folders.back());
			folders.pop_back();

			buf = fmt::dir::join(folders);
			stem = buf;
		}

		stem = path.substr(0, path.find(sys::sep::dir, stem.size() + 1));

		while (not empty(stack))
		{
			folders.push_back(stack.top());
			stack.pop();

			buf = fmt::dir::join(folders);
			auto const c = buf.data();
			if (file::fail(sys::mkdir(c, S_IRWXU)))
			{
				sys::err(here, "mkdir", c);
				stem = "";
				break;
			}
		}

		return stem;
	}

	bool remove_dir(fmt::string::view dir)
	{
		std::deque<fmt::string> deque;
		deque.emplace_back(dir);

		for (auto it = deque.begin(); it != deque.end(); ++it)
		{
			(void) find(*it, [&](fmt::string::view u)
			{
				auto const path = fmt::dir::join({*it, u});
				auto const c = path.data();
				struct sys::stat st(c);
				if (file::fail(st))
				{
					sys::err(here, "stat", c);
				}
				else
				if (S_ISDIR(st.st_mode))
				{
					if (u != "." and u != "..")
					{
						deque.emplace_back(move(path));
					}
				}
				else
				if (sys::fail(sys::unlink(c)))
				{
					sys::err(here, "unlink", c);
				}
				return success;
			});
		}

		bool ok = success;
		while (not empty(deque))
		{
			dir = deque.back();
			auto const c = dir.data();
			if (sys::fail(sys::rmdir(c)))
			{
				sys::err(here, "rmdir", c);
				ok = failure;
			}
			deque.pop_back();
		}
		return ok;
	}

	// pipe.hpp

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

	// fifo.hpp

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

	// socket.hpp

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

	// shm.hpp

	map_ptr make_map(int fd, size_t sz, off_t off, mode am, size_t *out)
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

#ifdef test_unit
#include "arg.hpp"

test_unit(mode)
{
	assert(not env::file::fail(__FILE__) and "Source file exists");
	assert(not env::file::fail(env::opt::arg(), env::file::ex) and "Program is executable");
}

test_unit(dir)
{
	assert(not env::file::fail(env::temp()));
	assert(not env::file::fail(env::pwd()));
	assert(not env::file::fail(env::home()));

	auto const path = fmt::dir::split(__FILE__);
	assert(not empty(path));
	auto const name = path.back();
	assert(not empty(name));
	auto const program = env::opt::program();
	assert(not empty(program));

	assert(env::file::find(env::pwd(), [program](auto entry)
	{
		return fmt::dir::split(entry).back() == program;
	}));

	auto const temp = fmt::dir::join({env::temp(), "my", "test", "dir"});
	if (std::empty(temp)) return;
	auto const stem = env::file::make_dir(temp);
//	assert(not empty(stem.first));
//	assert(not empty(stem.second));
	assert(not env::file::remove_dir(stem));
}

#endif
