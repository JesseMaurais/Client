#ifndef ipc_hpp
#define ipc_hpp

#include <initializer_list>
#include "form.hpp"
#include "str.hpp"
#include "ptr.hpp"
#include "run.hpp"

namespace env::file
{
	using run::command;
	using run::console;

	struct process : unique, form
	{
		bool open(size_t argc, char const** argv) noexcept;
		bool open(command line) noexcept;
		void close();
		bool stop();
		int wait();
		int dump();

		ssize_t error(char *str, size_t sz) const
		{
			control buf = cmd.second[2];
			return buf.read(str, sz);
		}

		ssize_t read(void *buf, size_t sz) const override
		{
			control buf = cmd.second[1];
			return buf.read(buf, sz);
		}

		ssize_t write(const void *buf, size_t sz) const override
		{
			control buf = cmd.second[0];
			return buf.write(buf, sz);
		}

	protected:

		console com;
	};

	struct work : process
	{
		~work()
		{
			(void) close();
		}

		int get(int fd[3] = nullptr) const
		{
			if (nullptr != fd)
			{
				for (int n : { 0, 1, 2 })
				{
					fd[n] = cmd.second[n].get();
				}
			}
			return pid;
		}
	};
}

#endif // file
