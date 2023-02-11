// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "file.hpp"
#include "mode.hpp"
#include "dir.hpp"
#include "ptr.hpp"
#include "sys.hpp"
#include "sync.hpp"
#include "type.hpp"
#ifdef _WIN32
#include "win/file.hpp"
#include "win/memory.hpp"
#else
#include "uni/fcntl.hpp"
#include "uni/mman.hpp"
#endif
#include <climits>

namespace env::file
{
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

	fmt::string to_string(int mask)
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

	bool fail(view u, mode mask)
	{
		if (not fmt::terminated(u))
		{
			auto buf = fmt::to_string(u);
			return fail(buf, mask);
		}

		#ifdef _WIN32
		if (DWORD dw; mask & ex)
		{
			return GetBinaryType(u.data(), &dw)
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

			return sys::access(u.data(), flags);
		}

		struct sys::stats state(u.data());
		if (sys::fail(state.ok))
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
					perror("fclose");
				}
			}
		});
	}

	unique_ptr open(view u, mode mask)
	{
		if (not fmt::terminated(u))
		{
			auto buf = fmt::to_string(u);
			return open(u, mask);
		}

		#ifdef assert
		assert((mask & (rwx|ok|un|app|fifo|bin|txt)) == mask);
		#endif

		if (mask & ex)
		{
			basic_ptr f = nullptr;

			if (mask & wr)
			{
				f = sys::popen(u.data(), "w");
			}
			else
			{
				f = sys::popen(u.data(), "r");
			}

			if (nullptr == f)
			{
				perror("popen");
			}

			return fwd::make_unique<FILE>(f, [](auto f)
			{
				if (f and sys::fail(sys::pclose(f)))
				{
					perror("pclose");
				}
			});
		}
		else
		if (mask & fifo)
		{

		}
		else
		{
			const auto mode = to_string(mask);
			auto f = std::fopen(u.data(), mode.data());
			if (nullptr == f)
			{
				perror("fopen");
			}
			return enclose(f);
		}

		return fwd::make_unique<FILE>(nullptr, [](auto){});
	}

	unique_ptr lock(basic_ptr f, mode mask, off_t off, size_t sz)
	{
		#ifdef assert
		assert(nullptr != f);
		assert((mask & (rw|ok)) == mask);
		#endif

		const auto fd = sys::fileno(f);
		#ifdef alert
		alert(sys::fail(fd));
		#endif

		#ifdef _WIN32
		{
			const auto h = sys::win::get(fd);
			#ifdef assert
			assert(not sys::win::fail(h));
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
				#ifdef WINERR
				WINERR("LockFileEx");
				#endif
			}
			else return fwd::make_unique<FILE>(f, [=](basic_ptr)
			{
				sys::win::overlapped over = off;
				if (not UnlockFileEx(h, 0, large.low_part(), large.high_part(), &over))
				{
					#ifdef WINERR
					WINERR("UnlockFileEx");
					#endif
				}
			});
		}
		#else // UNIX
		{
			sys::uni::lock key;
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
				perror((mask & wr) ? "F_WRLCK" : "F_RDLCK");
			}
			else return fwd::make_unique<FILE>(f, [=](basic_ptr)
			{
				sys::uni::lock key;
				key.l_type = F_UNLCK;
				key.l_whence = SEEK_SET;
				key.l_start = off;
				key.l_len = sz;
				if (key.set(fd))
				{
					perror("F_UNLCK");
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
		alert(sys::fail(fd));
		#endif

		if (0 == sz)
		{
			struct sys::stats st(fd);
			if (sys::fail(st.ok))
			{
				perror("stat");
			}
			else sz = st.st_size;
		}

		#ifdef _WIN32
		{
			const auto h = sys::win::get(fd);
			#ifdef assert
			assert(not sys::win::fail(h));
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
				#ifdef WINERR
				WINERR("CreateFileMapping");
				#endif
			}

			return sys::win::mem::map<char>(hm, flags, off, sz, buf);
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

	fmt::string path(basic_ptr f)
	{
		fmt::string buf;

		#ifdef assert
		assert(nullptr != f);
		#endif

		const int fd = sys::fileno(f);
		if (sys::fail(fd))
		{
			perror("fileno");
		}

		#ifdef _WIN32
		{
			const auto h = sys::win::get(fd);

			#ifdef assert
			assert(not sys::win::fail(h));
			#endif

			union
			{
				FILE_NAME_INFO info;
				char data[sizeof info + MAX_PATH];
			};

			if (not GetFileInformationByHandleEx(h, FileNameInfo, data, sizeof data))
			{
				#ifdef WINERR
				WINERR("GetFileInformationByHandleEx FileNameInfo", fd);
				#endif
			}
			else
			{
				fmt::wide w(info.FileName, info.FileNameLength);
				buf = fmt::to_string(w);
			}
		}
		#elif defined(F_GETPATH)
		{
			buf.resize(MAXPATHLEN);
			if (sys::fail(fcntl(fd, F_GETPATH, buf.data())))
			{
				perror("F_GETPATH");
				buf.clear();
			}
			else
			{
				const auto pos = buf.find_last_of("\0");
				if (fmt::npos != pos) buf.resize(pos);
			}
		}
		#elif defined(__bsdi__) || defined(__linux__)
		{
			using namespace std::literals;
			static const auto pid = sys::getpid();
			const auto link = "/proc/"s +fmt::to_string(pid) + "/fd/" + fmt::to_string(fd);

			buf.resize(PATH_MAX);
			const auto n = readlink(link.c_str(), buf.data(), buf.size());
			if (sys::fail(n))
			{
				perror("readlink");
				buf.clear();
			}
			else
			{
				buf.resize(n);
			}
		}
		#else
		#error Cannot implement function
		#endif

		return buf;
	}
}

#ifdef TEST
#include "arg.hpp"
TEST(mode)
{
	ASSERT(not env::file::fail(__FILE__) and "Source file exists");
	ASSERT(not env::file::fail(env::opt::arg(), env::file::ex) and "Program is executable");
}
TEST(flags)
{
	using namespace env::file;
	ASSERT(to_string(rw|un) == "w+");
	ASSERT(to_string(rd) == "r");
	ASSERT(to_string(rw|ok) == "x+");
}
TEST(lock)
{
	auto f = env::file::open(__FILE__);
	ASSERT(f and "Open file");
	auto rdk = env::file::lock(f.get(), env::file::rd);
	ASSERT(rdk and "Lock file to read");
	auto wrk = env::file::lock(f.get(), env::file::wo);
	ASSERT(not wrk and "Lock file to write");
}
#endif
