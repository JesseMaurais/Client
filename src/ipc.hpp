#ifndef ipc_hpp
#define ipc_hpp

#include <initializer_list>
#include "file.hpp"
#include "str.hpp"
#include "ptr.hpp"

namespace env::file
{
	using command = std::initializer_list<fmt::view>;
	using console = std::pair<int, int[3]>;

	console start(size_t argc, char const** argv) noexcept;
	console start(command line) noexcept;
	void close(console);
	bool stop(console);
	int wait(console);
	int dup(console);

	struct process : unique, form
	{
		process() = default;

		explicit process(size_t argc, char const** argv)
		: process(start(argc, argv))
		{ }

		explicit process(command line) 
		: process(start(line))
		{ }

		explicit process(console com)
		{
			set(com.first, com.second);
		}

		operator console() const
		{
			console pair;
			pair.first = get(pair.second);
			return pair;
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

		int get(int fd[3] = nullptr) const
		{
			if (nullptr != fd)
			{
				for (int n : { 0, 1, 2 })
				{
					fd[n] = fds[n].get();
				}
			}
			return pid;
		}

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

	protected:

		descriptor fds[3];
		int pid = invalid;
	};

	struct viewer : process
	{
		using process::process;

		~viewer()
		{
			set(); // leave open
		}
	};
}

#endif // file
