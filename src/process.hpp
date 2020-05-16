#ifndef process_hpp
#define process_hpp "File Process"

#include "pipe.hpp"
#include "ptr.hpp"
#include "fmt.hpp"

namespace env::file
{
	struct process : fwd::unique, stream
	{
		bool start(fmt::string::view::init args);
		bool start(fmt::string::view::span args);
		bool start(size_t argc, char const **argv);
		bool quit();
		bool wait();

		explicit process(fmt::string::view::init args)
		{
			(void) start(args);
		}

		explicit process(fmt::string::view::span args)
		{
			(void) start(args);
		}

		explicit process(size_t argc, char const** argv)
		{
			(void) start(argc, argv);
		}

		explicit process(int pid = invalid, int fd[3] = nullptr)
		{
			(void) set(pid, fd);
		}

		~process()
		{
			(void) quit();
		}

		ssize_t read(void *buf, size_t sz) const override
		{
			return fd[1].read(buf, sz);
		}

		ssize_t write(const void *buf, size_t sz) const override
		{
			return fd[0].write(buf, sz);
		}

		void close(int n = invalid)
		{
			if (fail(n)) 
			{
				for (auto n : { 0, 1, 2 })
				{
					close(n);
				}
			}
			else fd[n].close();
		}

		int set(int pid = invalid, int fd[3] = nullptr)
		{
			for (auto n : { 0, 1, 2 })
			{
				if (nullptr != fd)
				{
					fd[n] = this->fd[n].set(fd[n]);
				}
				else
				{
					this->fd[n].set();
				}
			}
			auto tmp = this->pid;
			this->pid = pid;
			return tmp;
		}

		int get(int fd[3] = nullptr) const
		{
			if (nullptr != fd)
			{
				for (auto n : { 0, 1, 2 })
				{
					fd[n] = this->fd[n].get();
				}
			}
			return pid;
		}

	protected:

		int pid = invalid;
		descriptor fd[3];
	};
}

#endif // file
