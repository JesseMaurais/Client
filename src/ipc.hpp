#ifndef ipc_hpp
#define ipc_hpp

#include "form.hpp"
#include "str.hpp"
#include "ptr.hpp"

namespace env::file
{
	using command = fmt::span_view;

	struct console : unique, form
	{
		ssize_t read(void *buf, size_t sz) const override;
		ssize_t write(const void *buf, size_t sz) const override;
		int start(size_t argc, char const** argv);
		int start(command line);
		int close(int pfd = -1);
		int quit();
		int wait();
		int dump();

	protected:

		int pid, fd[3];
	};

	struct process : console
	{
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
			(void) close();
			(void) quit();
		}
	};
}

#endif // file
