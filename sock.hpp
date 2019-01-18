#ifndef sock_hpp
#define sock_hpp

#include "file.hpp"

namespace sys::socket
{
	union address;
}

namespace sys::file
{
	struct socket
	{
		using address = sys::socket::address;

		operator bool() const;
		virtual ~socket();
		socket(int family, int type, int proto = 0);
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
