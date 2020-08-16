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

		ssize_t error(void *buf, size_t sz) const
		{
			return fd[2].read(buf, sz);
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
				for (auto m : { 0, 1, 2 })
				{
					close(m);
				}
			}
			else fd[n].close();
		}

		int set(int p = invalid, int f[3] = nullptr)
		{
			for (auto n : { 0, 1, 2 })
			{
				if (nullptr != f)
				{
					f[n] = fd[n].set(f[n]);
				}
				else
				{
					fd[n].set();
				}
			}
			auto tmp = p;
			pid = p;
			return tmp;
		}

		int get(int f[3] = nullptr) const
		{
			if (nullptr != f)
			{
				for (auto n : { 0, 1, 2 })
				{
					f[n] = fd[n].get();
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
