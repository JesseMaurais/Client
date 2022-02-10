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
	template <class object> struct exclusive_ptr : fwd::no_copy
	// Allow one writer but many readers 
	{
		rwlock lock;
		object *that;

		exclusive_ptr(object *ptr) : that(ptr)
		{
			#ifdef assert
			assert(nullptr != that);
			#endif
		}

		auto read()
		{
			using reader = decltype(lock.read());
			struct unlock : fwd::no_copy
			{
				reader const key;
				object const *that;

				unlock(exclusive_ptr* ptr)
				: key(ptr->lock.read())
				, that(ptr->that)
				{
					assert(that);
				}

				operator object const*() const
				{
					return that;
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
			struct unlock : fwd::no_copy
			{
				writer const key;
				object *that;

				unlock(exclusive_ptr *ptr)
				: key(ptr->lock.write())
				, that(ptr->that)
				{
					assert(that);
				}

				operator object const*() const
				{
					return that;
				}

				operator object*()
				{
					return that;
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

		auto unique() const
		{
			return fwd::make_unique(that, [that, key=read()](auto ptr)
			{
				#ifdef assert
				assert(that == ptr);
				#endif
			});
		}

		auto shared() const
		{
			return fwd::make_shared(that, [that, key=read()](auto ptr)
			{
				#ifdef assert
				assert(that == ptr);
				#endif
			});
		}

		auto unique()
		{
			return fwd::make_unique(that, [that, key=write()](auto ptr)
			{
				#ifdef assert
				assert(that == ptr);
				#endif
			});
		}

		auto shared()
		{
			return fwd::make_shared(that, [that, key=write()](auto ptr)
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
		{
			#ifdef assert
			assert(nullptr != that);
			#endif
		}

		auto read()
		{
			return that.read();
		}

		auto write()
		{
			return that.write();
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

	template <class Type> extern sys::exclusive<Type> extern_ptr;

	template <class Type> class atomic : public fwd::variable<Type>
	{
		mutable sys::rwlock lock;
		Type value;

	public:

		atomic(Type x = {}) : value(x)
		{ }

		operator Type() const final
		{
			auto const unlock = lock.read();
			return value;
		}

		Type operator=(Type n) final
		{
			auto const unlock = lock.write();
			return value = n;
		}
	};
}

#endif
