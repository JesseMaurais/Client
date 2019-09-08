// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "shm.hpp"
#include "sys.hpp"
#include "err.hpp"
#include "fmt.hpp"
#include <climits>
#include <map>

#ifdef _WIN32
# include "win/memory.hpp"
# include "win/file.hpp"
#else
# include "uni/mman.hpp"
# include "uni/shm.hpp"
#endif

namespace sys::file
{
	memory make_shm(int fd, size_t sz, off_t off, mode mask, size_t *out)
	{
		assert(not sys::fail(fd));
		assert(mask & sys::file::rwx);

		if (nullptr == out) out = &sz;

		#ifdef _WIN32
		{
			DWORD flags = 0;
			DWORD prot = 0;

			if (mask & ex)
			{
				flags |= FILE_MAP_EXECUTE;

				if (mask & sys::file::wr)
				{
					prot = PAGE_EXECUTE_READWRITE;
					flags |= FILE_MAP_WRITE;
				}
				else
				if (mask & sys::file::rd)
				{
					prot = PAGE_EXECUTE_READ;
					flags |= FILE_MAP_READ;
				}
			}
			else
			{
				if (mode & sys::file::wr)
				{
					prot = PAGE_READWRITE;
					flags |= FILE_MAP_WRITE;
				}
				else
				if (mask & sys::file::rd)
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

			return sys::win::make_map(h, flags, off, sz);
		}
		#else
		{
			if (0 == sz)
			{
				class sys::stat st(fd);
				if (sys::fail(st))
				{
					sys::err(here, "stat");
				}
				else sz = st.st_size;
			}
			*out = sz;

			#ifdef _POSIX_MAPPED_FILES
			{
				int prot = 0;
				if (mask & sys::file::rd) prot |= PROT_READ;
				if (mask & sys::file::wr) prot |= PROT_WRITE;
				if (mask & sys::file::ex) prot |= PROT_EXEC;
				return sys::uni::make_map(sz, prot, MAP_PRIVATE, fd, off);
			}
			#else
			{
				auto key = IPC_PRIVATE;

				if (not sys::fail(fd))
				{
					char path[PATH_MAX];
					if (sys::fail(fcntl(fd, F_GETPATH, path)))
					{
						sys::err(here, "F_GETPATH", fd);
					}

					key = ftok(path, 'f');
					if (sys::fail(key))
					{
						sys::err(here, "ftok", path);
					}
				}

				int flags = 0;
				if (mode & sys::file::ok) flags |= IPC_CREAT;
				if (mode & sys::file::xu) flags |= IPC_EXCL;
				auto const id = sys::uni::shm::get(sz, flags, key);
				return sys::uni::shm::at(id, flags);
			}
			#endif
		}
		#endif
	}
}

