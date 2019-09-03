#ifndef err_hpp
#define err_hpp

#ifdef assert
# undef assert
# warning You should not include assert. 
#endif

#ifdef NDEBUG
# define alert(x)
# define assert(x)
# define verify(x) (x)
#else
# define alert(x) if (bool(x)) sys::err(here, #x)
# define verify(x) assert(x)
# define assert(x) if (not(x)) sys::warn(here, #x)
#endif

#include <system_error>
#include <exception>
#include <sstream>
#include "str.hpp"

// Pessimistic boolean

enum : bool
{
	success = false, failure = true
};

constexpr bool fail(bool ok)
{
	return failure == ok;
}

namespace fmt
{
	struct where
	{
		char const *file; int const line; char const *func;
	};

	std::ostream& operator<<(std::ostream&, where const &);
	std::ostream& operator<<(std::ostream&, std::errc const &);
	std::ostream& operator<<(std::ostream&, std::exception const &);

	template <typename Arg, typename... Args>
	auto err(Arg arg, Args... args)
	{
		std::stringstream ss;
		ss << arg;
		if constexpr (0 < sizeof...(args))
		{
			((ss << " " << args), ...);
		}
		return ss.str();
	}
}

#define here ::fmt::where { __FILE__, __LINE__, __func__ }

namespace sys
{
	extern bool debug;

	inline std::errc const noerr { };

	bool iserr(std::errc ec);

	namespace impl
	{
		using fmt::string_view;
		void warn(string_view);
		void err(string_view);
	}

	template <typename... Args>
	inline void warn(Args... args)
	{
		if (debug)
		{
			impl::warn(fmt::err(args...));
		}
	}

	template <typename... Args>
	inline void err(Args... args)
	{
		if (debug)
		{
			impl::err(fmt::err(args...));
		}
	}
}

#endif // file
