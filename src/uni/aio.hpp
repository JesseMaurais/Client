#ifndef uni_aio_hpp
#define uni_aio_hpp "POSIX Asynchonous Input/Output"

#include "signal.hpp"
#include <functional>

namespace sys::uni::aio
{
	struct event : fwd::unique, fwd::zero<aiocb>
	{
		using function = sig::event::function;

		event(function f, pthread_attr_t* attr) : work(f)
		{
			aio_sigevent.sigev_value.sival_ptr = this;
			aio_sigevent.sigev_notify = SIGEV_THREAD;
			aio_sigevent.sigev_notify_function - thread;
			aio_sigevent.sigev_notify_attributes = attr;
		}

		bool read()
		{
			return fail(aio_read(this)) and err(here);
		}

		bool write()
		{
			return fail(aio_write(this)) and err(here);
		}

		bool error()
		{
			return fail(aio_error(this)) and err(here);
		}

		bool fsync(int op)
		{
			return fail(aio_fsync(op, this)) and err(here, op);
		}

		bool cancel(int fd)
		{
			return fail(aio_cancel(fd, this)) and err(here, fd);
		}

		auto yield()
		{
			return aio_return(this);
		}

	private:

		function work;

		static void thread(sigval sv)
		{
			auto that = fwd::cast_as<event>(sv.sival_ptr);
			if (that) that->work();
		}
	};
};

#endif // file
