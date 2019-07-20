#ifndef file_hpp
#define file_hpp

#include <cstddef>
#include <ios>

namespace sys::file
{
	using size_t = std::size_t;
	using ssize_t = std::ptrdiff_t;
	using openmode = std::ios_base::openmode;

	constexpr openmode
		app   = std::ios_base::app,
		bin   = std::ios_base::binary,
		in    = std::ios_base::in,
		out   = std::ios_base::out,
		trunc = std::ios_base::trunc,
		ate   = std::ios_base::ate,
		io    = std::ios_base::in | std::ios_base::out;

	int convert(openmode); // C++ to POSIX

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

		void open(char const* path, openmode mode);
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
