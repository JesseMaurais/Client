#ifndef file_hpp
#define file_hpp

#include "str.hpp"
#include "ptr.hpp"
#include "ops.hpp"

namespace sys::file
{
	// Access modes
	enum mode : int
	{
		ex   = 1 << 0, // execute
		wr   = 1 << 1, // write
		rd   = 1 << 2, // read
		ok   = 1 << 3, // exists
		xu   = 1 << 4, // exclusive
		sz   = 1 << 5, // truncate
		app  = 1 << 6, // append
		txt  = 1 << 7, // text
		bin  = 1 << 8, // binary
		rw   = rd | wr,
		wo   = rw | ok,
		rwx  = rw | ex,
		oz   = wo | sz,
		ox   = wo | ex,
	};

	constexpr int owner(int am)
	{
		return (am & rwx) << 6;
	}

	constexpr int group(int am)
	{
		return (am & rwx) << 3;
	}

	constexpr int other(int am)
	{
		return (am & rwx) << 0;
	}

	// Permissions
	enum permit : int
	{
		owner_r = owner(rd),
		owner_w = owner(wr),
		owner_x = owner(ex),
		group_r = group(rd),
		group_w = group(wr),
		group_x = group(ex),
		other_r = other(rd),
		other_w = other(wr),
		other_x = other(ex),
	};

	inline permit owner(mode am)
	{
		return static_cast<permit>(owner(static_cast<int>(am)));
	}

	inline permit group(mode am)
	{
		return static_cast<permit>(group(static_cast<int>(am)));
	}

	inline permit other(mode am)
	{
		return static_cast<permit>(other(static_cast<int>(am)));
	}

	int check(mode); // file access mode
	int convert(mode); // file open mode
	int convert(permit); // permissions

	// Check for access to the file at path
	bool fail(fmt::view path, mode am = ok);

	// Query or change data buffer size
	size_t bufsiz(ssize_t sz = invalid);

	// Scoped file descriptor
	struct descriptor : unique, ops
	{
		explicit descriptor(fmt::view path, mode am = rw, permit pm = owner(rw));
		ssize_t write(const void *buf, size_t sz) const override;
		ssize_t read(void *buf, size_t sz) const override;
		bool close();

		explicit descriptor(int fd = invalid)
		{
			(void) set(fd);
		}

		~descriptor()
		{
			if (not sys::fail(fd))
			{
				(void) close();
			}
		}

		int set(int value = invalid)
		{
			int const tmp = fd;
			fd = value;
			return tmp;
		}

		int get() const
		{
			return fd;
		}

	protected:

		int fd;
	};

	struct pipe : unique, ops
	{
		explicit pipe();

		explicit pipe(int fd[2])
		{
			set(fd);
		}

		void set(int fd[2] = nullptr)
		{
			for (int n : { 0, 1 })
			{
				fds[n].set(fd ? fd[n] : invalid);
			}
		}

		void get(int fd[2]) const
		{
			for (int n : { 0, 1 })
			{
				fd[n] = fds[n].get();
			}
		}

		ssize_t read(void* buf, size_t sz) const override
		{
			return fds[0].read(buf, sz);
		}

		ssize_t write(const void* buf, size_t sz) const override
		{
			return fds[1].write(buf, sz);
		}

		const descriptor& operator[](size_t n) const
		{
			return fds[n];
		}

		descriptor& operator[](size_t n)
		{
			return fds[n];
		}

	protected:

		descriptor fds[2];
	};
}

#endif // file
