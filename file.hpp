#ifndef file_hpp
#define file_hpp

#include <initializer_list>
#include <cstdint>
#include <ios>

namespace sys::socket
{
	union address;
}

namespace sys::file
{
	using size_t = std::size_t;
	using ssize_t = std::ptrdiff_t;
	using openmode = std::ios_base::openmode;
	using arguments = std::initializer_list<char const*>;
	
	extern size_t bufsiz;

	constexpr auto app   = std::ios_base::app;
	constexpr auto bin   = std::ios_base::binary;
	constexpr auto in    = std::ios_base::in;
	constexpr auto out   = std::ios_base::out;
	constexpr auto trunc = std::ios_base::trunc;
	constexpr auto ate   = std::ios_base::ate;

	template <typename T>
	constexpr bool fail(T const value)
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
			int const tmp = this->fd;
			this->fd = fd;
			return tmp;
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

		operator bool() const;
		virtual ~socket();
		socket(int family, int type, int proto);
		socket accept(address& name, size_t *length = nullptr) const;
		bool connect(address const& name, size_t length) const;
		bool bind(address const& name, size_t length) const;
		bool listen(int backlog) const;
		bool shutdown(int how) const;


		template <typename T>
		inline ssize_t send(T const* data, size_t size, int flags) const
		{
			return send(static_cast<const void*>(data), size, flags);
		}
		template <typename T>
		inline ssize_t send(T const* data, size_t size, int flags, address const& name, size_t length) const
		{
			return send(static_cast<const void*>(data), size, flags, name, length);
		}

		template <typename T>
		inline ssize_t receive(T* data, size_t size, int flags) const
		{
			return receive(static_cast<T*>(data), size, flags);
		}
		template <typename T>
		inline ssize_t receive(T* data, size_t size, int flags, address& name, size_t& length) const
		{
			return receive(static_cast<T*>(data), size, flags, name, length);
		}

	private:

		ssize_t send(const void *data, size_t size, int flags) const;
		ssize_t send(const void *data, size_t size, int flags, address const& name, size_t length) const;

		ssize_t receive(void *data, size_t size, int flags) const;
		ssize_t receive(void *data, size_t size, int flags, address& name, size_t& length) const;

	protected:

		socket(std::intptr_t s);
		std::intptr_t s;
	};
}

namespace sig
{
	struct socket : sys::file::socket
	{	
		socket(int family, int type, int proto, int events);
		~socket();

		virtual void notify(int event) = 0;
		static int poll(int timeout = -1);
	};
}

#endif // file
