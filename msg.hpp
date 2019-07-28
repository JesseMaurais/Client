#ifndef msg_hpp
#define msg_hpp

#include <memory>
#include <functional>

namespace sys::file
{
	struct descriptor;
	struct socket;
	struct fifo;
}

namespace sys::msg
{
	using receiver = std::function<int(short)>;

	struct poll
	{
		using namespace sys::file;

		poll();
		bool wait(int timeout = -1);
		void add(int fd, short ev, receiver);
		void add(descriptor const& fd, short ev, receiver);
		void add(socket const& s, short ev, receiver);

	pivate:

		struct impl;
		std::uniqe_ptr<impl> ptr;
	};
}

#endif // file
