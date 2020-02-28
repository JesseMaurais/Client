#ifndef proc_hpp
#define proc_hpp

#include "pipe.hpp"
#include "str.hpp"
#include "ptr.hpp"

namespace env::file
{
	using command = fmt::list_view;

	struct process : unique, stream
	{
		int start(size_t argc, char const** argv);
		int start(command line);
		int quit();
		int wait();
		int dump();

		explicit process(size_t argc, char const** argv)
		{
			(void) start(argc, argv);
		}

		explicit process(command line)
		{
			(void) start(line);
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
				for (int n : { 0, 1, 2 })
				{
					close(n);
				}
			}
			else fd[n].close();
		}

		int get(int fds[3]) const
		{
			for (int n : { 0, 1, 2 })
			{
				fds[n] = fd[n].get();
			}
			return pid;
		}

	protected:

		int pid = invalid;
		descriptor fd[3];
	};
}

#endif // file
