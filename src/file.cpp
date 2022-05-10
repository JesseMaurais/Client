// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "file.hpp"
#include "mode.hpp"
#include "ptr.hpp"
#include "err.hpp"
#include "sys.hpp"
#include "sync.hpp"
#include "type.hpp"
#ifdef _WIN32
#include "win/file.hpp"
#else
#include "uni/fcntl.hpp"
#include "uni/mman.hpp"
#endif
#include <climits>

namespace env::file
{
	fwd::variable<size_t>& width()
	{
		static sys::atomic<size_t> safe = BUFSIZ;
		return safe;
	}

	int to_flags(mode mask)
	{
		#ifdef assert
		assert((mask & (rw|ok|un|xu|app|txt|bin)) == mask);
		#endif

		int flags = 0;

		if ((mask & rw) == rw)
		{
			flags |= O_RDWR;
		}
		else
		if (mask & wr)
		{
			flags |= O_WRONLY;
		}
		else
		if (mask & rd)
		{
			flags |= O_RDONLY;
		}

		if (mask & txt)
		{
			flags |= O_TEXT;
		}
		else
		if (mask & bin)
		{
			flags |= O_BINARY;
		}

		if (mask & app)
		{
			flags |= O_APPEND;
		}

		if (mask & un)
		{
			flags |= O_TRUNC;
		}

		if (mask & xu)
		{
			flags |= O_EXCL;
		}

		if (mask & ok)
		{
			flags |= O_CREAT;
		}

		return flags;
	}

	int to_flags(permit mask)
	{
		int flags = 0;

		if (mask & owner_x)
		{
			flags |= S_IXUSR;
		}
		if (mask & owner_w)
		{
			flags |= S_IWUSR;
		}
		if (mask & owner_r)
		{
			flags |= S_IRUSR;
		}
		if (mask & group_x)
		{
			flags |= S_IXGRP;
		}
		if (mask & group_w)
		{
			flags |= S_IWGRP;
		}
		if (mask & group_r)
		{
			flags |= S_IRGRP;
		}
		if (mask & other_x)
		{
			flags |= S_IXOTH;
		}
		if (mask & other_w)
		{
			flags |= S_IWOTH;
		}
		if (mask & other_r)
		{
			flags |= S_IROTH;
		}

		return flags;
	}

	fmt::string to_string(mode mask)
	{
		#ifdef assert
		assert((mask & (rw|un|ok|app|bin|txt)) == mask);
		#endif

		fmt::string buf;

		if (mask & un)
		{
			if (mask & rd)
			{
				buf = "w+";
			}
			else
			{
				buf = "w";
			}
		}
		else
		if (mask & ok)
		{
			if (mask & rd)
			{
				buf = "x+";
			}
			else
			{
				buf = "x";
			}
		}
		else
		if (mask & app)
		{
			if (mask & rd)
			{
				buf = "a+";
			}
			else
			{
				buf = "a";
			}
		}
		else
		{
			if (mask & wr)
			{
				buf = "r+";
			}
			else
			{
				buf = "r";
			}
		}

		if (mask & bin)
		{
			buf += 'b';
		}

		if (mask & txt)
		{
			buf += 't';
		}

		return buf;
	}

	bool fail(view path, mode mask)
	{
		if (not fmt::terminated(path))
		{
			return fail(fmt::to_string(path), mask);
		}

		#ifdef _WIN32
		if (DWORD dw; mask & ex)
		{
			return GetBinaryType(path.data(), &dw)
				? success : failure;
		}
		#endif

		if ((mask & rwx) == mask)
		{
			int flags = 0;

			if (mask & ok)
			{
				flags |= F_OK;
			}
			if (mask & ex)
			{
				flags |= X_OK;
			}
			if (mask & rd)
			{
				flags |= R_OK;
			}
			if (mask & wr)
			{
				flags |= W_OK;
			}

			return sys::access(path.data(), flags);
		}

		struct sys::stats state(path.data());
		if (fail(state.ok))
		{
			return failure;
		}

		if (mask & dir)
		{
			#ifdef S_ISDIR
			if (not S_ISDIR(state.st_mode))
			#endif
				return failure;
		}
		if (mask & chr)
		{
			#ifdef S_ISCHAR
			if (not S_ISCHR(state.st_mode))
			#endif
				return failure;
		}
		if (mask & reg)
		{
			#ifdef S_ISREG
			if (not S_ISREG(state.st_mode))
			#endif
				return failure;
		}
		if (mask & fifo)
		{
			#ifdef S_ISFIFO
			if (not S_ISFIFO(state.st_mode))
			#endif
				return failure;
		}
		if (mask & sock)
		{
			#ifdef S_IFSOCK
			if (not S_ISSOCK(state.st_mode))
			#endif
				return failure;
		}
		if (mask & blk)
		{
			#ifdef S_ISFBLK
			if (not S_ISFBLK(state.st_mode)
			#endif
				return failure;
		}
		if (mask & reg)
		{
			#ifdef S_ISREG
			if (not S_ISREG(state.st_mode))
			#endif
				return failure;
		}
		if (mask & lnk)
		{
			#ifdef S_ISLNK
			if (not S_ISLNK(state.st_mode))
			#endif
				return failure;
		}

		return success;
	}

	unique_ptr enclose(basic_ptr f)
	{
		return fwd::make_unique<FILE>(f, [](auto f)
		{
			if (nullptr != f)
			{
				if (EOF == std::fclose(f))
				{
					sys::err(here, "fclose");
				}
			}
		});
	}

	unique_ptr open(view path, mode mask)
	{
		if (not fmt::terminated(path))
		{
			return open(fmt::to_string(path), mask);
		}

		#ifdef assert
		assert((mask & (rwx|ok|un|app|fifo|bin|txt)) == mask);
		#endif

		if (mask & ex)
		{
			basic_ptr f = nullptr;

			if (mask & wr)
			{
				f = sys::popen(path.data(), "w");
			}
			else
			{
				f = sys::popen(path.data(), "r");
			}

			if (nullptr == f)
			{
				sys::err(here, "popen", path);
			}

			return fwd::make_unique<FILE>(f, [](auto f)
			{
				if (f and sys::fail(sys::pclose(f)))
				{
					sys::err(here, "pclose");
				}
			})
		}
		else
		if (mask & fifo)
		{
			const auto name = fmt::file::fifo(path);

			#ifdef _WIN32
			{

			}
			#else // UNIX
			{
				const auto flags = to_flags(mask & rw);
				if (sys::fail(mkfifo(name.data(), flags)))
				{
					sys::err(here, "mkfifo", name);
				}
				else
				{
					return open(name, mask & ~fifo);
				}
			}
			#endif // API
		}
		else
		{
			const auto mode = to_string(mask);
			auto f = std::fopen(path.data(), mode.data());
			if (nullptr == f)
			{
				sys::err(here, "fopen", mode, path);
			}
			return enclose(f);
		}
	}

	unique_ptr lock(basic_ptr f, mode mask, size_t off, size_t sz)
	{
		#ifdef assert
		assert(nullptr != f);
		assert((mask & (rw|ok)) == mask);
		#endif

		const auto fd = sys::fileno(f);
		#ifdef alert
		alert(fail(fd));
		#endif

		#ifdef _WIN32
		{
			const auto h = sys::win::get(fd);
			#ifdef assert
			assert(not sys::win::fail(h))
			#endif

			if (0 == sz)
			{
				sz = SIZE_MAX;
			}

			DWORD dw = 0;
			if (mask & wr)
			{
				dw |= LOCKFILE_EXCLUSIVE_LOCK;
			}
			if (mask & ok)
			{
				dw |= LOCKFILE_FAIL_IMMEDIATELY;
			}

			sys::win::overlapped over = off;
			sys::win::large_int large = sz;

			if (not LockFileEx(h, dw, 0, large.low_part(), large.high_part(), &over))
			{
				sys::win::err(here, "LockFileEx");
			}
			else return fwd::make_ptr(f, [=](basic_ptr)
			{
				if (not UnlockFileEx(h, 0, large.low_part(), large.high_part(), &over))
				{
					sys::win::err(here, "UnlockFileEx");
				}
			});
		}
		#else // UNIX
		{
			sys::uni::file::lock key;
			key.l_whence = SEEK_SET;
			key.l_start = off;
			key.l_len = sz;

			if (mask & wr)
			{
				key.l_type = F_WRLCK;
			}
			else
			{
				key.l_type = F_RDLCK;
			}

			if ((mask & ok) ? key.set(fd) : key.wait(fd))
			{
				sys::err(here, (mask & wr) ? "F_WRLCK" : "F_RDLCK", fd);
			}
			else return fwd::make_unique<FILE>(f, [=](basic_ptr)
			{
				key.l_type = F_UNLCK;
				if (key.set(fd))
				{
					sys::err(here, "FD_UNLCK", fd);
				}
			});
		}
		#endif

		return nullptr;
	}

	unique_buf map(basic_ptr f, mode mask, off_t off, size_t sz, basic_buf buf)
	{
		#ifdef assert
		assert(nullptr != f);
		assert((mask & (rwx|xu)) == mask);
		#endif

		const auto fd = sys::fileno(f);
		#ifdef alert
		alert(fail(fd));
		#endif

		if (0 == sz)
		{
			struct sys::stats st(fd);
			if (sys::fail(st.ok))
			{
				sys::err(here, "stat");
			}
			else sz = st.st_size;
		}

		#ifdef _WIN32
		{
			const auto h = sys::win::get(fd);
			#ifdef assert
			assert(not sys::win::fail(h))
			#endif

			DWORD flags = 0;
			DWORD prot = 0;

			if (mask & xu)
			{
				flags |= FILE_MAP_COPY;
			}

			if (mask & ex)
			{
				flags |= FILE_MAP_EXECUTE;

				if ((mask & rw) == rw)
				{
					prot = PAGE_EXECUTE_READWRITE;
					flags |= FILE_MAP_ALL_ACCESS;
				}
				else
				if (mask & wr)
				{
					prot = (mask & xu) ? PAGE_EXECUTE_WRITECOPY : PAGE_EXECUTE_READWRITE;
					flags |= FILE_MAP_WRITE;
				}
				else
				if (mask & rd)
				{
					prot = PAGE_EXECUTE_READ;
					flags |= FILE_MAP_READ;
				}
			}
			else
			{
				if ((mask & rw) == rw)
				{
					prot = PAGE_READWRITE;
					flags |= FILE_MAP_ALL_ACCESS;
				}
				else
				if (mask & wr)
				{
					prot = (mask & xu) ? PAGE_WRITECOPY : PAGE_READWRITE;
					flags |= FILE_MAP_WRITE;
				}
				else
				if (mask & rd)
				{
					prot = PAGE_READONLY;
					flags |= FILE_MAP_READ;
				}
			}

			sys::win::handle const hm = CreateFileMapping
			(
				h, nullptr, prot, HIWORD(sz), LOWORD(sz), nullptr
			);

			if (sys::win::fail(hm))
			{
				sys::win::err(here, "CreateFileMapping");
			}

			return sys::win::mem::map(hm, flags, off, sz, buf);
		}
		#else
		{
			int prot = 0;
			int flags = 0;

			if (mask & rd)
			{
				prot |= PROT_READ;
			}
			if (mask & wr)
			{
				prot |= PROT_WRITE;
			}
			if (mask & ex)
			{
				prot |= PROT_EXEC;
			}

			if (mask & xu)
			{
				flags |= MAP_PRIVATE;
			}
			else
			{
				flags |= MAP_SHARED;
			}

			return sys::uni::shm::map(sz, prot, flags, fd, off, buf);
		}
		#endif
	}

	fmt::string name(basic_ptr f)
	{
		fmt::string path;

		#ifdef assert
		assert(nullptr != f)
		#endif

		const int fd = sys::fileno(f);

		#ifdef alert
		alert(sys::fail(fd));
		#endif

		#ifdef _WIN32
		{
			const auto h = sys::win::get(fd);

			#ifdef assert
			assert(not sys::win::fail(h));
			#endif

			union
			{
				FILE_NAME_INFO info;
				char buf[sizeof info + MAX_PATH];
			};

			if (not GetFileInformationByHandleEx(h, FileNameInfo, buf, sizeof buf))
			{
				sys::win::err(here, "GetFileInformationByHandleEx FileNameInfo", fd);
			}
			else
			{
				fmt::wide w(info.FileName, info.FileNameLength);
				path = fmt::to_string(w);
			}
		}
		#elif defined(F_GETPATH)
		{
			path.resize(MAXPATHLEN);
			if (sys::fail(fcntl(fd, F_GETPATH, path.data())))
			{
				sys::err(here, "F_GETPATH", fd);
				path.clear();
			}
			else
			{
				const auto pos = path.find_last_of("\0");
				if (fmt::npos != pos) path.resize(pos);
			}
		}
		#elif defined(__bsd__) || defined(__linux__)
		{
			static const auto pid = sys::getpid();
			const auto link = "/proc/"s + pid + "/fd/" + fmt::to_string(fd);

			path.resize(MAXLEN)
			const auto n = readlink(link, path.data(), path.size());
			if (sys::fail(n))
			{
				sys::err(here, "readlink", link);
				path.clear();
			}
			else
			{
				path.resize(n);
			}
		}
		#else
		#error Cannot implement function
		#endif

		return path;
	}
}

#ifdef test_unit
#include "arg.hpp"
test_unit(mode)
{
	assert(not env::file::fail(__FILE__) and "Source file exists");
	assert(not env::file::fail(env::opt::arg(), env::file::ex) and "Program is executable");
}
test_unit(flags)
{
	using namespace env::file;
	assert(to_flags(rw|un) == "w+");
	assert(to_flags(rd) == "r");
	assert(to_flags(rw|ok) == "x+");
}
test_unit(lock)
{
	auto f = env::file::open(__FILE__);
	assert(f and "Open file");
	auto rdk = env::file::lock(f.get(), env::file::rd);
	assert(rdk and "Lock file to read");
	auto wrk = env::file::lock(f.get(), env::file::wo);
	assert(not wrk and "Lock file to write");
}
#endif