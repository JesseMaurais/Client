// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "time.hpp"
#include "type.hpp"
#include "meta.hpp"
#include "doc.hpp"
#include <iomanip>

#ifdef _WIN32
#include "win/sync.hpp"
#else
#include "uni/signal.hpp"
#include "uni/time.hpp"
#endif

namespace doc
{
	template <> std::string_view name<&std::tm::tm_year> = "year";
	template <> std::string_view name<&std::tm::tm_mon> = "month";
	template <> std::string_view name<&std::tm::tm_mday> = "day-of-month";
	template <> std::string_view name<&std::tm::tm_wday> = "day-of-week";
	template <> std::string_view name<&std::tm::tm_yday> = "day-of-year";
	template <> std::string_view name<&std::tm::tm_hour> = "hour";
	template <> std::string_view name<&std::tm::tm_min> = "minute";
	template <> std::string_view name<&std::tm::tm_sec> = "seconds";
	template <> std::string_view name<&std::tm::tm_isdst> = "daylight-saving-time";
	template <> std::string_view name<&std::timespec::tv_sec> = "seconds";
	template <> std::string_view name<&std::timespec::tv_nsec> = "nanoseconds";
}

namespace env
{
	time::time(int base)
	{
		if (std::timespec_get(this, base))
		{
			sys::warn(here, "timespec_get", base);
		}
	}

	fmt::access date::operator()(fmt::view format)
	{
		return
		{
			[=](fmt::input in)->fmt::input
			{
				const auto s = fmt::to_string(format);
				return in >> std::get_time(this, s.data());
			}
			,
			[=](fmt::output out)->fmt::output
			{
				const auto s = fmt::to_string(format);
				return out << std::put_time(this, s.data());
			}
		};
	}

	gmtime::gmtime(std::time_t t)
	{
		#ifdef _MSC_VER
		{
			if (auto n = std::gmtime_s(this, &t); n)
			{
				_set_errno(n);
				sys::err(here, "gmtime_s");
			}
		}
		#else
		{
			#ifdef __STDC_LIB_EXT1__
			if (gmtime_s(&t, this))
			#else
			if (gmtime_r(&t, this))
			#endif
			{
				sys::err(here, "gmtime_s");
			}
		}
		#endif
	}

	localtime::localtime(std::time_t t)
	{
		#ifdef _MSC_VER
		{
			if (auto n == std::localtime_s(this, &t); n)
			{
				_set_errno(n);
				sys::err(here, "localtime_s");
			}
		}
		#else
		{
			#ifdef __STDC_LIB_EXT1__
			if (localtime_s(&t, this))
			#else
			if (localtime_r(&t, this))
			#endif
			{
				sys::err(here, "localtime_s");
			}
		}
		#endif
	}

	void wait(fmt::time tv)
	{
		#ifdef _WIN32
		{
			const auto div = std::div(tv.tv_nsec, 1e6L);
			const auto msec = std::fma(tv.tv_sec, 1e3L, div.quot);
			#ifdef assert
			assert(0 == div.rem);
			#endif
			Sleep(msec);
		}
		#else
		{
			const auto div = std::div(tv.tv_nsec, 1e3L);
			const auto usec = std::fma(tv.tv_sec, 1e6L, div.quot);
			#ifdef assert
			assert(0 == div.rem);
			#endif
			if (sys::fail(usleep(usec)))
			{
				sys::err(here, "usleep");
			}
		}
		#endif
	}

	fwd::pop event(fmt::timer it, fwd::event f)
	{
		#ifdef _WIN32
		{
			const auto t = sys::win::timer::convert(it.it_value);
			const auto p = sys::win::timer::convert(it.it_interval);
			#ifdef assert
			assert(0 == t.rem);
			assert(0 == p.rem);
			#endif
			const auto h = sys::sync().reader()->timer();
			return [t=sys::win::timer(h, f, t.quot, p.quot)] { };
		}
		#else
		{
			struct intern : sys::uni::time::event
			{
				intern(fmt::timer it, fwd::event f) : event(f)
				{
					sys::uni::timer(it.it_interval, it.it_value).set(id);
				}
			};
			return [t=intern(it, f)] { };
		}
		#endif
	}
}

#ifdef test_unit
test_unit(time)
{
	// Attempt to convert a number of common and standard time formats
	const auto format = fmt::split("%n %t %c %x %X %D %F %R %T %Z %%");
	// Check that the whitespace split works
	assert(format.size() == 9);
	// Use each format to convert local time
	env::localtime local;
	for (auto f : format)
	{
		std::stringstream ss;
		ss << local(f);
		auto s = ss.str();
		assert(not s.empty());
	}
}
#endif