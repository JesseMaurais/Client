#ifndef file_hpp
#define file_hpp

#include <cstddef>
#include <ios>

namespace sys::net
{
	union address;
}

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
		ate   = std::ios_base::ate;

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

		int set(int fd_ = -1)
		{
			int const tmp = fd;
			fd = fd_;
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

		bool execute(char const** argv);
		void terminate();
		int wait();

		void close(int n)
		{
			file[n].close();
		}

	protected:

		descriptor file[3];
		std::intptr_t pid;
	};

	struct socket
	{
		using address = sys::net::address;

		operator bool() const;
		virtual ~socket();
		socket(int family, int type, int proto = 0);
		socket accept(address& name, size_t *length = nullptr) const;
		bool connect(address const& name, size_t length) const;
		bool bind(address const& name, size_t length) const;
		bool listen(int backlog) const;
		bool shutdown(int how) const;

		template <typename T>
		inline ssize_t write(T const* data, size_t size, int flags) const
		{
			return write(static_cast<const void*>(data), size, flags);
		}

		template <typename T>
		inline ssize_t write(T const* data, size_t size, int flags, address const& name, size_t length) const
		{
			return write(static_cast<const void*>(data), size, flags, name, length);
		}

		template <typename T>
		inline ssize_t read(T* data, size_t size, int flags) const
		{
			return read(static_cast<T*>(data), size, flags);
		}

		template <typename T>
		inline ssize_t read(T* data, size_t size, int flags, address& name, size_t& length) const
		{
			return read(static_cast<T*>(data), size, flags, name, length);
		}

	private:

		ssize_t write(const void *data, size_t size, int flags) const;
		ssize_t write(const void *data, size_t size, int flags, address const& name, size_t length) const;

		ssize_t read(void *data, size_t size, int flags) const;
		ssize_t read(void *data, size_t size, int flags, address& name, size_t& length) const;

	protected:

		socket(int fd);
		int fd;
	};
}

namespace sig
{
	struct socket : sys::file::socket
	{	
		socket(int family, int type, int proto, short events);
		~socket();

		virtual void notify(short events) = 0;
		static int poll(int timeout = -1);
	};
}

#endif // file
