#ifndef file_hpp
#define file_hpp

#include <utility>
#include <string>
#include <ios>
#include "any.hpp"

namespace sys::socket
{
	union address;
}

namespace sys::file
{
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

	template <typename T> constexpr bool fail(T const value)
	{
		constexpr T invalid = -1;
		return invalid == value;
	}

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

	struct socket
	{
		using address = sys::socket::address;

		socket();
		socket(int family, int type, int proto);
		~socket();

		operator bool();
		socket accept(address& name, size_t *length = nullptr);
		bool connect(address const& name, size_t length);
		bool bind(address const& name, size_t length);
		bool listen(int backlog);

		ssize_t send(char const *data, size_t size, int flags);
		ssize_t send(char const *data, size_t size, int flags, address const& name, size_t length);

		ssize_t receive(char *data, size_t size, int flags);
		ssize_t receive(char *data, size_t size, int flags, address const& name, size_t length);

	private:

		socket(std::intptr_t s);
		std::intptr_t s;
	};
}

#endif // file
