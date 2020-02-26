#ifndef ipc_hpp
#define ipc_hpp

#include "form.hpp"
#include "str.hpp"
#include "ptr.hpp"

namespace env::file
{
	struct process : unique, form
	{
		ssize_t read(void *buf, size_t sz) const override;
		ssize_t write(const void *buf, size_t sz) const override;
		int start(size_t argc, char const** argv);
		int start(fmt::span_view args);
		int close(int pfd = -1);
		int raise(int signo);
		int wait();
		int dump();

	protected:

		int pid, fd[3];
	};
}

#endif // file
