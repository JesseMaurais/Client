#ifndef file_hpp
#define file_hpp

#include "ops.hpp"
#include "str.hpp"

namespace sys::file
{
	using fmt::string_view;

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

	int access(mode);
	int convert(mode);
	int convert(permit);

	bool fail(string_view path, mode am = ok);

	extern size_t const bufsiz;

	struct descriptor : ops
	{
		descriptor(int fd = invalid)
		{
			(void) set(fd);
		}

		descriptor(string_view path, mode am = rw, permit pm = owner(rw))
		{
			(void) open(path, am, pm);
		}

		~descriptor()
		{
			if (not sys::fail(fd))
			{
				(void) close();
			}
		}

		int set(int newfd = invalid)
		{
			int const tmp = fd;
			fd = newfd;
			return tmp;
		}

		int get() const
		{
			return fd;
		}

		bool open(string_view path, mode am = rw, permit pm = owner(rw));
		ssize_t write(const void* buf, size_t sz) const override;
		ssize_t read(void* buf, size_t sz) const override;
		bool close();

	protected:

		int fd;
	};

	struct pipe : ops
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

	struct process : ops
	{
		~process()
		{
			if (not sys::fail(pid))
			{
				(void) close();
			}
		}

		void set(int fd[3] = nullptr)
		{
			for (int n : { 0, 1, 2 })
			{
				fds[n].set(fd ? fd[n] : invalid);
			}
		}

		void get(int fd[3]) const
		{
			for (int n : { 0, 1, 2 })
			{
				fd[n] = fds[n].get();
			}
		}

		auto get() const
		{
			return pid;
		}

		ssize_t err(void* buf, size_t sz) const
		{
			return fds[2].read(buf, sz);
		}

		ssize_t read(void* buf, size_t sz) const override
		{
			return fds[1].read(buf, sz);
		}

		ssize_t write(const void* buf, size_t sz) const override
		{
			return fds[0].write(buf, sz);
		}

		const descriptor& operator[](size_t n) const
		{
			return fds[n];
		}

		descriptor& operator[](size_t n)
		{
			return fds[n];
		}

		bool execute(char const** argv);
		bool kill();
		int wait();

		bool close(int n)
		{
			return fds[n].close();
		}

		void close()
		{
			for (int n : { 0, 1, 2 })
			{
				int fd = fds[n].get();
				if (not sys::fail(fd))
				{
					(void) fds[n].close();
				}
			}
		}

	protected:

		descriptor fds[3];
		int pid = invalid;
	};
}

#endif // file
