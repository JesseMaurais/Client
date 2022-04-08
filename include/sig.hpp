#ifndef sig_hpp
#define sig_hpp "Signals and Sockets"

#include <map>
#include <csignal>
#include <functional>
#include <algorithm>
#include "fmt.hpp"

namespace sys::sig
{
	using string = fmt::string::view;
	using slot = fwd::sig::slot<string>;
	using socket = slot::socket;
	using function = slot::function;
	using signature = slot::signature;

	using basic_ptr = fwd::as_ptr<signature>;
	using unique_ptr = fwd::unique_ptr<signature>;
	using shared_ptr = fwd::shared_ptr<signature>;
	using weak_ptr = fwd::weak_ptr<signature>;

	inline bool fail(basic_ptr ptr)
	{
		return SIG_ERR == ptr;
	}

	inline bool fault(basic_ptr ptr)
	{
		return SIG_DFL == ptr;
	}

	inline bool ignore(basic_ptr ptr)
	{
		return SIG_IGN == ptr;
	}

	inline bool empty(basic_ptr ptr)
	{
		return fail(ptr) or fault(ptr) or ignore(ptr);
	}

	void raise(string);
	void notify(string, function);
}

#endif // file
