#ifndef ipc_hpp
#define ipc_hpp

#include <initializer_list>
#include "form.hpp"
#include "str.hpp"
#include "ptr.hpp"

namespace env::file
{
	template <size_t n = 3>
	using console = std::pair<int, int[n]>;
	using command = std::initializer_list<fmt::view>;

	struct process : unique, form, console
	{
		bool start(size_t argc, char const** argv) noexcept;
		bool start(command line) noexcept;
		void close();
		bool stop();
		int wait();
		int dump();

		explicit process(size_t argc, char const** argv)
		{ 
			(void) cmd.start(argc, argv);
		}

		explicit process(command line)
		{
			(void) con.start(line);
		}

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

	protected:

		console cmd;
	};
}

#endif // file
