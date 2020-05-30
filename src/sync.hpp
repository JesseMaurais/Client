#ifndef sync_hpp
#define sync_hpp "Thread Synchronize"

#include "env.hpp"
#ifdef _WIN32
#include "win/sync.hpp"
#else
#include "uni/pthread.hpp"
#endif

namespace sys
{
	extern env::view::ref thread_id;

	template <class object> struct exclusive_ptr : fwd::unique
	// Allow one writer but many readers (WORM pattern)
	{
		rwlock lock;
		object *that;

		exclusive_ptr(object *ptr) : that(ptr)
		{ }

		auto read()
		{
			using reader = decltype(lock.read());
			struct unlock : fwd::unique
			{
				reader const key;
				object const *that;

				unlock(exclusive_ptr* ptr)
				: key(ptr->lock.read())
				, that(ptr->that)
				{ }

				auto const& operator*() const
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
			using writer = decltype(lock.write());
			struct unlock : fwd::unique
			{
				writer const key;
				object *that;

				unlock(exclusive_ptr *ptr)
				: key(ptr->lock.write())
				, that(ptr->that)
				{ }

				auto const& operator*() const
				{
					return *that;
				}

				auto& operator*()
				{
					return *that;
				}

				auto operator->() const
				{
					return that;
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
		template <class> friend class cache;
		exclusive_ptr<object> that;

	public:

		using object::object;

		exclusive() : that(this)
		{ }

		auto read()
		{
			return that.read();
		}

		auto write()
		{
			return that.write();
		}
	};

	template <class object> class cache : object
	// Copy with read and write back
	{
		using exclusive = exclusive<object>;
		using exclusive_ptr = exclusive_ptr<object>;

		exclusive_ptr *that;

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
