#include "err.hpp"
#include "sync.hpp"
#include <cassert> // overwrite assert

fmt::string::out fmt::operator<<(string::out out, std::errc const& ec)
{
	return out << std::make_error_condition(ec).message();
}

fmt::string::out fmt::operator<<(string::out out, std::exception const& ex)
{
	return out << ex.what();
}

fmt::string::out fmt::operator<<(string::out out, fmt::string::view::init where)
{
	assert(where.size() == 3);
	auto const at = where.begin();
	return out << at[0] << '(' << at[1] << ')' << at[2] << ':';
}

namespace
{
	struct : env::variable<fmt::string::view>
	{
		thread_local type id = fmt::none;

		operator type() const final
		{
			return id;
		}

		type operator=()(type value) final
		{
			return id = value;
		}

	} THREAD_ID;
}

namespace sys
{
	env::view:ref thread_id = THREAD_ID;

	bool debug =
	#ifdef NDEBUG
		false;
	#else
		true;
	#endif

	thread_local fmt::string::stream local;

	fmt::string::out::ref out()
	{
		return local;
	}

	void flush() 
	{
		static sys::mutex key;
		auto const unlock = key.lock();
		fmt::string line;
		while (std::getline(local, line))
		{
			std::cerr << line << std::endl;
		}
	}

	int bug(fmt::string::view message, bool no)
	{
		thread_local struct
		{
			fmt::string last;
			int counter = -1;
		} local;
		// Ouptut stream
		auto & put = out();
		// Avoid spamming
		if (message != local.last)
		{
			// reset
			local.counter = 0;
			local.last = fmt::to_string(message);
			// format
			{
				// message
				put << local.last;
				// number
				if (view err = std::strerror(errno); no)
				{
					put << ':' << ' ' << err;
				}
				// thread
				if (view id = thread_id; not std::empty(id))
				{
					put << '[' << id << ']';
				}
			}
			put << '\n';
		}
		else ++local.counter;
		return local.counter;
	}
}

