#ifndef pipe_hpp
#define pipe_hpp

#include "fd.hpp"

namespace env::file
{
	struct pipe : unique, stream
	{
		explicit pipe();

		explicit pipe(int fd[2])
		{
			set(fd);
		}

		void set(int fd[2] = nullptr)
		{
			for (auto n : { 0, 1 })
			{
				fds[n].set(fd ? fd[n] : invalid);
			}
		}

		void get(int fd[2]) const
		{
			for (auto n : { 0, 1 })
			{
				fd[n] = fds[n].get();
			}
		}

		ssize_t read(void* buf, size_t sz) const override
		{
			return fds[0].read(buf, sz);
		}

		ssize_t write(const void* buf, size_t sz) const override
		{
			return fds[1].write(buf, sz);
		}

		const descriptor& operator[](size_t n) const
		{
			return fds[n];
		}

		descriptor& operator[](size_t n)
		{
			return fds[n];
		}

	protected:

		descriptor fds[2];
	};
}

#endif // file
