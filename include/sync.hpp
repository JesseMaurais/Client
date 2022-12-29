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
				leave();
				#ifdef assert
				assert(this == that);
				#endif
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
				unlock();
				#ifdef assert
				assert(this == that);
				#endif
			});
		}

		auto writer()
		{
			xlock();
			return share_this([this](auto that)
			{
				xunlock();
				#ifdef assert
				assert(this == that);
				#endif
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
				unlock();
				#ifdef assert
				assert(this == that);
				#endif
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
				unlock();
				#ifdef assert
				assert(this == that);
				#endif
			});
		}

		auto writer()
		{
			wrlock();
			return share_this([this](auto that)
			{
				unlock();
				#ifdef assert
				assert(this == that);
				#endif
			});
		}
	};

#endif

	template <class object> class exclusive_ptr : fwd::no_copy
	// Allow one writer but many readers
	{
		mutable rwlock lock;
		object* that;

	public:

		exclusive_ptr(object* ptr) : that(ptr)
		{
			#ifdef assert
			assert(nullptr != ptr);
			#endif
		}

		auto reader() const
		{
			return fwd::make_shared<object>(that, [this, key=lock.reader()](auto that)
			{
				#ifdef assert
				assert(this->that == that);
				#endif
			});
		}

		auto writer()
		{
			return fwd::make_shared<object>(that, [this, key=lock.writer()](auto that)
			{
				#ifdef assert
				assert(this->that == that);
				#endif
			});
		}

		auto get() const
		{
			return that;
		}

		auto get()
		{
			return that;
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

		auto get() const
		{
			return that.get();
		}

		auto get()
		{
			return that.get();
		}
	};

	template <class Type> extern exclusive<Type> extern_ptr;
}

#endif
