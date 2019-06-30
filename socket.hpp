#ifndef socket_hpp
#define socket_hpp

#include "file.hpp"

namespace sys::net
{
	union address;
}

namespace sys::file
{
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
