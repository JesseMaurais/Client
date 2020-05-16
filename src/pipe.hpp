#ifndef pipe_hpp
#define pipe_hpp "Communication Pipes"

#include "fmt.hpp"
#include "ptr.hpp"
#include "file.hpp"
#include "mode.hpp"

namespace env::file
{
	struct descriptor : fwd::unique, stream
	{
		ssize_t read(void *buf, size_t sz) const override;
		ssize_t write(const void *buf, size_t sz) const override;
		bool open(fmt::string::view path, mode = rw, permit = owner(rw));
		bool close();

		explicit descriptor(fmt::string::view path, mode am = rw, permit pm = owner(rw))
		{
			(void) open(path, am, pm);
		}

		explicit descriptor(int fd = invalid)
		{
			(void) set(fd);
		}

		~descriptor()
		{
			if (not fail(fd))
			{
				(void) close();
			}
		}

		int set(int value = invalid)
		{
			int const tmp = fd;
			fd = value;
			return tmp;
		}

		int get() const
		{
			return fd;
		}

	protected:

		int fd;
	};

	struct pipe : fwd::unique, stream
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
