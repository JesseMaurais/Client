#ifndef file_hpp
#define file_hpp

#include "ops.hpp"

namespace sys::file
{
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

	bool fail(char const* path, mode um = ok);

	constexpr int owner(int bit)
	{
		return (bit & rwx) << 6;
	}

	constexpr int group(int bit)
	{
		return (bit & rwx) << 3;
	}

	constexpr int other(int bit)
	{
		return (bit & rwx) << 0;
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

	inline permit owner(mode bit)
	{
		return static_cast<permit>(owner(static_cast<int>(bit)));
	}

	inline permit group(mode bit)
	{
		return static_cast<permit>(group(static_cast<int>(bit)));
	}

	inline permit other(mode bit)
	{
		return static_cast<permit>(other(static_cast<int>(bit)));
	}

	int access(mode);
	int convert(mode);
	int convert(permit);
	
	extern size_t const bufsiz;

	struct descriptor : ops
	{
		descriptor(int fd = invalid)
		{
			(void) set(fd);
		}

		descriptor(char const *path, mode um = wo, permit pm = owner(rw))
		{
			(void) open(path, um, pm);
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

		bool open(char const* path, mode bit = wo, permit pm = owner(rw));
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
		process() : pid(invalid)
		{
			set();
		}

		~process()
		{
			if (not sys::fail(pid))
			{
				close();
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

		bool execute(char const** argv);
		bool kill();
		int wait();

		void close(int n)
		{
			fds[n].close();
		}

		void close()
		{
			for (int n : { 0, 1, 2})
			{
				fds[n] = invalid;
			}
		}

	protected:

		descriptor fds[3];
		long pid;
	};
}

#endif // file
