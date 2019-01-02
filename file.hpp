#ifndef file_hpp
#define file_hpp

#include <utility>
#include <string>
#include <ios>

namespace sys
{
	template <typename I> constexpr bool fail(I const value)
	{
		constexpr I invalid = -1;
		return invalid == value;
	}
}

namespace sys::file
{
	using sys::fail;
	using size_t = std::size_t;
	using ssize_t = std::make_signed<size_t>::type;
	using openmode = std::ios_base::openmode;
	using arguments = std::initializer_list<char const *>;

	extern size_t bufsiz;

	constexpr auto app   = std::ios_base::app;
	constexpr auto bin   = std::ios_base::binary;
	constexpr auto in    = std::ios_base::in;
	constexpr auto out   = std::ios_base::out;
	constexpr auto trunc = std::ios_base::trunc;
	constexpr auto ate   = std::ios_base::ate;

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

		int set(int fd = -1)
		{
			std::swap(fd, this->fd);
			return fd;
		}

		int get() const
		{
			return fd;
		}

		operator bool() const
		{
			return fail(fd);
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

		void open(std::string const& path, openmode mode);
		void close();

	private:

		ssize_t write(const void* buffer, size_t size) const;
		ssize_t read(void* buffer, size_t size) const;

	protected:

		int fd;
	};

	struct pipe
	{
		pipe();

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
		explicit process(int fd[3] = nullptr)
		{
			set(fd);
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

		operator bool() const
		{
			for (int n : { 0, 1, 2 })
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

		bool execute(arguments);
		void terminate();

	protected:

		descriptor file[3];
		std::intptr_t pid;
	};
}

#endif // file
