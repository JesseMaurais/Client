#ifndef socket_hpp
#define socket_hpp

#include "file.hpp"
#include "ops.hpp"

namespace sys::net
{
	union address;
}

namespace sys::file
{
	struct socket : ops
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

		ssize_t read(void* buf, size_t sz, int flags) const;
		ssize_t read(void* buf, size_t sz, int flags, address& addr, size_t& len) const;
		ssize_t read(void* buf, size_t sz) const override
		{
			return read(buf, sz, 0);
		}

		template <typename C>
		ssize_t read(C* buf, size_t sz, int flags) const
		{
			return read(static_cast<void*>(buf), sz, flags);
		}

		template <typename C>
		ssize_t read(C* buf, size_t sz, int flags, address& name, size_t& length) const
		{
			return read(static_cast<void*>(buf), sz, flags, name, length);
		}

		ssize_t write(const void* buf, size_t sz, int flags) const;
		ssize_t write(const void* buf, size_t sz, int flags, address const& addr, size_t len) const;
		ssize_t write(const void* buf, size_t sz) const override
		{
			return write(buf, sz, 0);
		}

		template <typename C>
		ssize_t write(const C* buf, size_t sz, int flags) const
		{
			return write(static_cast<const void*>(buf), sz, flags);
		}

		template <typename C>
		ssize_t write(const C* buf, size_t sz, int flags, address const& name, size_t length) const
		{
			return write(static_cast<const void*>(buf), sz, flags, name, length);
		}

	protected:

		socket(int fd);
		int fd = invalid;
	};
}

#endif // file
