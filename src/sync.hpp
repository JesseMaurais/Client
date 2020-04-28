#ifndef sync_hpp
#define sync_hpp "Thread Point Sync"

#include "env.hpp"
#ifdef _WIN32
#include "win/sync.hpp"
#else
#include "uni/pthread.hpp"
#endif

namespace sys
{
	extern env::variable<fmt::string::view> thread_id;

	template <class object> class exclusive_ptr : unique
	// Allow one writer but many reader (WORM pattern)
	{
		rwlock lock;
		object *that;

		using reader = decltype(lock.read());
		using writer = decltype(lock.write());

	public:

		exclusive(object *ptr) : that(ptr)
		{ }

		auto read()
		{
			struct unlock : unique
			{
				reader const key;
				object const *that;

				unlock(exclusive *ptr)
				: that(ptr->that), key(that->lock.read())
				{ }

				auto operator*() const
				{
					return *that;
				}

				auto operator->() const
				{
					return that;
				}
			};
			return unlock(this);
		}

		auto write()
		{
			struct unlock : unique
			{
				writer const key;
				object *that;

				unlock(exclusive *ptr)
				: that(ptr->that), key(that->lock.write())
				{ }

				auto operator*() const
				{
					return *that;
				}

				auto operator->() const
				{
					return that;
				}

				auto &operator*() const
				{
					return *that;
				}

				auto operator->()
				{
					return that;
				}
			};
			return unlock(this);
		}
	};

	template <class object> class exclusive : object
	// Self exclusion object
	{
		friend class cache<object>;
		exclusive_ptr<object> that(this);

	public:

		using object::object;

		auto read()
		{
			return that->read();
		}

		auto write()
		{
			return that->write();
		}
	};

	template <class object> class cache : object
	// Copy with read and write back
	{
		using exclusive = exclusive<object>;
		using exclusive_ptr = exclusive_ptr<object>;

		exclusion_ptr *that;

		cache(exclusive *ptr) : cache(&ptr->that)
		{ }

		cache(exclusive_ptr *ptr) : that(ptr)
		{
			// non-blocking read
			*this = *that->read();
		}

		auto write()
		{
			// write blocking copy
			*that->write() = *this;
		}
	};
}

#endif
