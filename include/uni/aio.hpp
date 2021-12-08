#ifndef uni_aio_hpp
#define uni_aio_hpp "POSIX Asynchonous Input/Output"

#include "signal.hpp"
#include <functional>
#include <aio.h>

namespace sys::uni::aio
{
	struct event : fwd::no_copy, fwd::zero<aiocb>
	{
		using function = sig::event::function;

		event(function f, pthread_attr_t* attr = nullptr)
		{
			aio_sigevent.sigev_value.sival_int = doc::socket().open(f);
			aio_sigevent.sigev_notify = SIGEV_THREAD;
			aio_sigevent.sigev_notify_function = thread;
			aio_sigevent.sigev_notify_attributes = attr;
		}

		~event()
		{
			doc::socket().close(aio_sigevent.sigev_value.sival_int);
		}

		bool read()
		{
			return fail(aio_read(this)) and sys::err(here);
		}

		bool write()
		{
			return fail(aio_write(this)) and sys::err(here);
		}

		bool error()
		{
			return fail(aio_error(this)) and sys::err(here);
		}

		bool fsync(int op)
		{
			return fail(aio_fsync(op, this)) and sys::err(here, op);
		}

		bool cancel(int fd)
		{
			return fail(aio_cancel(fd, this)) and sys::err(here, fd);
		}

		auto yield()
		{
			return aio_return(this);
		}

	private:

		static void thread(sigval sigev_value)
		{
			doc::raise(sigev_value.sival_int);
		}
	};
};

#endif // file
