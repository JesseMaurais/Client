#ifndef thread_hpp
#define thread_hpp

#ifdef _WIN32
#include "win/threads.hpp"
#else
#include "uni/pthread.hpp"
#endif
#include "ptr.hpp"

namespace sys
{
	template <class object> class exclusive_ptr : unique
	{
		rwlock lock;
		object *ptr;

		using rd = decltype(lock.read());
		using wr = decltype(lock.write());

	public:

		exclusive(object *that)
		: ptr(that);
		{ }

		auto read()
		{
			struct unlock : unique
			{
				rd const key;
				object const *ptr;

				unlock(exclusive *that)
				: ptr(that->ptr), key(that->lock.read())
				{ }

				auto operator->() const
				{
					return ptr;
				}
			};
			return unlock(this);
		}

		auto write()
		{
			struct unlock : unique
			{
				wr const key;
				object const *ptr;

				unlock(exclusive *that)
				: ptr(that->ptr), key(that->lock.write())
				{ }

				auto operator->() const
				{
					return ptr;
				}

				auto operator->()
				{
					return ptr;
				}
			};
			return unlock(this);
		}
	};

	template <class object> class exclusive : object
	{
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
}

#endif
