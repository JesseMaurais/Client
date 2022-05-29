#ifndef sync_hpp
#define sync_hpp "Synchronize Threads"

#include "err.hpp"
#include "env.hpp"
#include "ptr.hpp"
#ifdef _WIN32
#include "win/sync.hpp"
#else
#include "uni/pthread.hpp"
#endif

namespace sys
{
	template <class object> class exclusive_ptr : fwd::no_copy
	// Allow one writer but many readers
	{
		mutable rwlock lock;
		object* that;

	public:

		exclusive_ptr(object* ptr) : that(ptr)
		{
			#ifdef assert
			assert(nullptr != that);
			#endif
		}

		auto reader() const
		{
			using reader = decltype(lock.reader());
			struct unlock : fwd::no_copy
			{
				const reader key;
				const object *that;

				unlock(const exclusive_ptr* ptr)
				: key(ptr->lock.reader())
				, that(ptr->that)
				{
					#ifdef assert
					assert(that);
					#endif
				}

				operator const object*() const
				{
					return that;
				}

				const object& operator*() const
				{
					return *that;
				}

				const object* operator->() const
				{
					return that;
				}
			};
			return unlock(this);
		}

		auto writer()
		{
			using writer = decltype(lock.writer());
			struct unlock : fwd::no_copy
			{
				const writer key;
				object* that;

				unlock(exclusive_ptr *ptr)
				: key(ptr->lock.writer())
				, that(ptr->that)
				{
					#ifdef assert
					assert(that);
					#endif
				}

				operator const object*() const
				{
					return that;
				}

				operator object*()
				{
					return that;
				}

				const object& operator*() const
				{
					return *that;
				}

				object& operator*()
				{
					return *that;
				}

				const object* operator->() const
				{
					return that;
				}

				object* operator->()
				{
					return that;
				}
			};
			return unlock(this);
		}

		auto unique() const
		{
			return fwd::make_unique(that, [this, key=reader()](auto ptr)
			{
				#ifdef assert
				assert(that == ptr);
				#endif
			});
		}

		auto shared() const
		{
			return fwd::make_shared(that, [this, key=reader()](auto ptr)
			{
				#ifdef assert
				assert(that == ptr);
				#endif
			});
		}

		auto unique()
		{
			return fwd::make_unique(that, [this, key=writer()](auto ptr)
			{
				#ifdef assert
				assert(that == ptr);
				#endif
			});
		}

		auto shared()
		{
			return fwd::make_shared(that, [this, key=writer()](auto ptr)
			{
				#ifdef assert
				assert(that == ptr);
				#endif
			});
		}
	};

	template <class object> class exclusive : object
	// Self exclusion object
	{
		exclusive_ptr<object> that;

	public:

		using object::object;

		exclusive() : that(this)
		{ }

		auto reader() const
		{
			return that.reader();
		}

		auto writer()
		{
			return that.writer();
		}

		auto unique() const
		{
			return that.unique();
		}

		auto shared() const
		{
			return that.shared();
		}

		auto unique()
		{
			return that.unique();
		}

		auto shared()
		{
			return that.shared();
		}
	};

	template <class Type> extern exclusive<Type> extern_ptr;
}

#endif
