#ifndef file_hpp
#define file_hpp

#include <cstddef>
#include <cstdint>
#include <initializer_list>

namespace sys::file
{
	using size_t = std::size_t;
	using ssize_t = std::ptrdiff_t;

	enum mode : int
	{
		run    = 1 << 0,
		write  = 1 << 1,
		read   = 1 << 2,
		extant = 1 << 3,
		only   = 1 << 4,
		erase  = 1 << 5,
		append = 1 << 6,
		text   = 1 << 7,
		binary = 1 << 8,
	};

	enum permit : int;

	constexpr auto owner(mode bit)
	{
		return static_cast<permit>((bit & 07) << 6);
	}

	constexpr auto group(mode bit)
	{
		return static_cast<permit>((bit & 07) << 3);
	}

	constexpr auto other(mode bit)
	{
		return static_cast<permit>((bit & 07) << 0);
	}

	enum permit : int
	{
		owner_r = owner(read),
		owner_w = owner(write),
		owner_x = owner(run),

		group_r = group(read),
		group_w = group(write),
		group_x = group(run),

		other_r = other(read),
		other_w = other(write),
		other_x = other(run),
	};

	int access(mode);
	int convert(mode);
	int convert(permit);

	constexpr int invalid = -1;

	inline bool fail(int fd)
	{
		return invalid == fd;
	}
	
	extern size_t bufsiz;

	struct descriptor
	{
		descriptor(int fd = invalid)
		{
			(void) set(fd);
		}

		descriptor(char const *path, mode mask)
		{
			open(path, mask);
		}

		~descriptor()
		{
			if (not fail(fd))
			{
				close();
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

		template <class C>
		ssize_t write(const C* buf, size_t sz) const
		{
			return write(static_cast<const void*>(buf), sz);
		}

		template <class C>
		ssize_t read(C* buf, size_t sz) const
		{
			return read(static_cast<void*>(buf), sz);
		}

		void open(char const* path, mode);
		void close();

	private:

		ssize_t write(const void* buf, size_t sz) const;
		ssize_t read(void* buf, size_t sz) const;

	protected:

		int fd = invalid;
	};

	inline bool fail(descriptor const& fd)
	{
		return fail(fd.get());
	}

	struct pipe
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

	inline bool fail(pipe const& fds)
	{
		for (int n : { 0, 1 })
		{
			if (fail(fds[n]))
			{
				return true;
			}
		}
		return false;
	}

	struct process
	{
		process() : pid(invalid)
		{
			set();
		}

		~process()
		{
			if (not fail(pid))
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

		const descriptor& operator[](size_t n) const
		{
			return fds[n];
		}

		descriptor& operator[](size_t n)
		{
			return fds[n];
		}

		void run(char const** argv);
		void quit();
		void kill();
		int wait();

		int join()
		{
			quit();
			return wait();
		}

		int stop()
		{
			kill();
			return wait();
		}

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

	inline bool fail(process const& pid)
	{
		return fail(pid.get());
	}
}

#endif // file
