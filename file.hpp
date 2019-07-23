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
		explicit descriptor(int fd = invalid)
		{
			set(fd);
		}

		~descriptor()
		{
			close();
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

		operator bool() const
		{
			return not fail(fd);
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

		int fd = -1;
	};

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
				file[n].set(fd ? fd[n] : -1);
			}
		}

		void get(int fd[2]) const
		{
			for (int n : { 0, 1 })
			{
				fd[n] = file[n].get();
			}
		}

		operator bool() const
		{
			for (int n : { 0, 1 })
			{
				if (file[n]) return true;
			}
			return false;
		}

		const descriptor& operator[](size_t n) const
		{
			return file[n];
		}

		descriptor& operator[](size_t n)
		{
			return file[n];
		}

	protected:

		descriptor file[2];
	};

	struct process
	{
		process() : pid(invalid)
		{
			set();
		}

		void set(int fd[3] = nullptr)
		{
			for (int n : { 0, 1, 2 })
			{
				file[n].set(fd ? fd[n] : -1);
			}
		}

		void get(int fd[3]) const
		{
			for (int n : { 0, 1, 2 })
			{
				fd[n] = file[n].get();
			}
		}

		auto get() const
		{
			return pid;
		}

		operator bool() const
		{
			return not fail(pid);
		}

		const descriptor& operator[](size_t n) const
		{
			return file[n];
		}

		descriptor& operator[](size_t n)
		{
			return file[n];
		}

		void run(char const** argv);
		void kill();
		int wait();
		int join()
		{
			kill();
			return wait();
		}

		void close(int n)
		{
			file[n].close();
		}

	protected:

		descriptor file[3];
		std::intptr_t pid;
	};
}

#endif // file
