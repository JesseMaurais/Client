#ifndef sync_hpp
#define sync_hpp "Synchronize Threads"

#include "err.hpp"
#include "ptr.hpp"
#ifdef _WIN32
#include "win/sync.hpp"
#else
#include "uni/pthread.hpp"
#endif

namespace sys
{

#ifdef _WIN32

	using thread = win::start;
	using join = win::join;

	struct mutex : private fwd::traits<win::critical_section, fwd::shared>
	{
		auto key()
		{
			enter();
			return share_this([this](auto that)
			{
				#ifdef assert
				assert(this == that);
				#endif
				leave();
			});
		}
	};

	struct rwlock : private fwd::traits<win::srwlock, fwd::shared>
	{
		auto reader()
		{
			lock();
			return share_this([this](auto that)
			{
				#ifdef assert
				assert(this == that);
				#endif
				unlock();
			});
		}

		auto writer()
		{
			xlock();
			return share_this([this](auto that)
			{
				#ifdef assert
				assert(this == that);
				#endif
				xunlock();
			});
		}
	};

#else // POSIX

	using thread = uni::start;
	using join = uni::join;

	struct mutex : private fwd::traits<uni::mutex, fwd::shared>
	{
		auto key()
		{
			lock();
			return share_this([this](auto that)
			{
				#ifdef assert
				assert(this == that);
				#endif
				unlock();
			});
		}
	};

	struct rwlock : private fwd::traits<uni::rwlock, fwd::shared>
	{
		auto reader()
		{
			rdlock();
			return share_this([this](auto that)
			{
				#ifdef assert
				assert(this == that);
				#endif
				unlock();
			});
		}

		auto writer()
		{
			wrlock();
			return share_this([this](auto that)
			{
				#ifdef assert
				assert(this == that);
				#endif
				unlock();
			});
		}
	};

#endif

	template <class object> class exclusive_ptr : fwd::no_copy
	// Allow one writer but many readers
	{
		mutable rwlock lock;
		object* ptr;

	public:

		exclusive_ptr(object* that) : ptr(that)
		{
			#ifdef assert
			assert(nullptr != ptr);
			#endif
		}

		auto unique_reader() const
		{
			return fwd::make_unique(ptr, [this, key=lock.reader()](auto that)
			{
				#ifdef assert
				assert(that == ptr);
				#endif
			});
		}

		auto unique_writer()
		{
			return fwd::make_unique(ptr, [this, key=lock.writer()](auto that)
			{
				#ifdef assert
				assert(that == ptr);
				#endif
			});
		}

		auto shared_reader() const
		{
			return fwd::make_shared(ptr, [this, key=lock.reader()](auto that)
			{
				#ifdef assert
				assert(that == ptr);
				#endif
			});
		}

		auto shared_writer()
		{
			return fwd::make_shared(ptr, [this, key=lock.writer()](auto that)
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

		auto unique_reader() const
		{
			return that.unique_reader();
		}

		auto unique_writer()
		{
			return that.unique_writer();
		}

		auto shared_reader() const
		{
			return that.shared_reader();
		}

		auto shared_writer()
		{
			return that.shared_writer();
		}
	};

	template <class Type> extern exclusive<Type> extern_ptr;
}

#endif
