#ifndef ipc_hpp
#define ipc_hpp

#include <initializer_list>
#include "file.hpp"
#include "str.hpp"
#include "ptr.hpp"

namespace sys::file
{
	using command = std::initializer_list<fmt::view>;
	
	struct process : unique, form
	{
		process(int id, int fd[3] = nullptr) { set(id, fd); }
		process(size_t argc, char const** argv);
		process(command line);
		bool kill();
		int wait();

		int set(int id = invalid, int fd[3] = nullptr)
		{
			for (int n : { 0, 1, 2 }) if (fd)
			{
				int tmp = fds[n].get();
				fds[n].set(fd[n]);
				fd[n] = tmp;
			}
			else fds[n].set();
			int tmp = pid;
			pid = id;
			return tmp;
		}

		int get(int fd[3]) const
		{
			for (int n : { 0, 1, 2 })
			{
				fd[n] = fds[n].get();
			}
			return pid;
		}

		ssize_t error(char *str, size_t sz) const
		{
			return fds[2].read(str, sz);
		}

		ssize_t read(void *buf, size_t sz) const override
		{
			return fds[1].read(buf, sz);
		}

		ssize_t write(const void *buf, size_t sz) const override
		{
			return fds[0].write(buf, sz);
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

		descriptor fds[3];
		int pid = invalid;
	};
}

#endif // file
