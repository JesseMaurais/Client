#ifndef sync_hpp
#define sync_hpp "Thread Synchronize"

#include "err.hpp"
#include "env.hpp"
#include "tmp.hpp"
#ifdef _WIN32
#include "win/sync.hpp"
#else
#include "uni/pthread.hpp"
#endif

namespace sys
{
	extern thread_local fmt::string::view thread_id;

	template <class object> struct exclusive_ptr : fwd::unique
	// Allow one writer but many readers (WORM pattern)
	{
		rwlock lock;
		object *that;

		exclusive_ptr(object *ptr) : that(ptr)
		{ 
			assert(that);
		}

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
				{ 
					assert(that);
				}

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
				{ 
					assert(that);
				}

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
		exclusive_ptr<object> that;

	public:

		using object::object;

		exclusive() : that(this)
		{ 
			assert(this);
		}

		auto read()
		{
			return that.read();
		}

		auto write()
		{
			return that.write();
		}
	};

	template <auto Value> class atomic : public fwd::variable<decltype(Value)>
	{
		using typename fwd::variable<decltype(Value)>::type;
		mutable sys::rwlock lock;
		type value = Value;

	public:

		operator type() const final
		{
			auto const unlock = lock.read();
			return value;
		}

		type operator=(type n) final
		{
			auto const unlock = lock.write();
			return value = n;
		}
	};
}

#endif
