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

	constexpr int owner(mode bit)
	{
		return (bit & 07) << 6;
	}

	constexpr int group(mode bit)
	{
		return (bit & 07) << 3;
	}

	constexpr int other(mode bit)
	{
		return (bit & 07) << 0;
	}

	enum permit : int
	{
		owner_x = owner(run),
		owner_w = owner(write),
		owner_r = owner(read),

		group_x = group(run),
		group_w = group(write),
		group_r = group(read),

		other_x = other(run),
		other_w = other(write),
		other_r = other(read),
	};

	int convert(mode);
	int openmode(mode);
	int convert(permit);

	template <typename T>
	constexpr bool fail(T const value)
	{
		constexpr T invalid = -1;
		return invalid == value;
	}
	
	extern size_t bufsiz;

	struct descriptor
	{
		explicit descriptor(int fd = -1)
		{
			set(fd);
		}

		~descriptor()
		{
			close();
		}

		int set(int newfd = -1)
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
		ssize_t write(const C* buffer, size_t size) const
		{
			return write(static_cast<const void*>(buffer), size);
		}

		template <class C>
		ssize_t read(C* buffer, size_t size) const
		{
			return read(static_cast<void*>(buffer), size);
		}

		void open(char const* path, mode);
		void close();

	private:

		ssize_t write(const void* buffer, size_t size) const;
		ssize_t read(void* buffer, size_t size) const;

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
		process() : pid(-1)
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

		bool run(char const** argv);
		void kill();
		int wait();

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
