#ifndef fs_hpp
#define fs_hpp

#if __has_include(<filesystem>)
#include <filesystem>
namespace sys::file
{
	using namespace std::filesystem;
	using error_code = std::error_code;

	constexpr auto 
		owner_in = perms::owner_read,
		owner_out = perms::owner_write,
		owner_exec = perms::owner_exec,
		owner_all = perms::owner_all,
		group_in = perms::group_read,
		group_out = perms::group_write,
		group_exec = perms::group_exec,
		group_all = perms::group_all,
		other_in = perms::others_read,
		other_out = perms::others_write,
		other_exec = perms::others_exec,
		other_all = perms::others_all,
		all_access = perms::all,
		no_access = perms::none;
}
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace sys::file
{
	using namespace std::experimental::filesystem;
	using error_code = std::error_code;

	constexpr auto 
		owner_in = perms::owner_read,
		owner_out = perms::owner_write,
		owner_exec = perms::owner_exec,
		owner_all = perms::owner_all,
		group_in = perms::group_read,
		group_out = perms::group_write,
		group_exec = perms::group_exec,
		group_all = perms::group_all,
		other_in = perms::others_read,
		other_out = perms::others_write,
		other_exec = perms::others_exec,
		other_all = perms::others_all,
		all_access = perms::all,
		no_access = perms::none;
}
#elif __has_include(<boost/filesystem.hpp>)
#include <boost/filesystem.hpp>
namespace sys::file
{
	using namespace boost::filesystem;
	using error_code = boost::system::error_code;

	constexpr auto 
		owner_in = perms::owner_read,
		owner_out = perms::owner_write,
		owner_exec = perms::owner_exe,
		owner_all = perms::owner_all,
		group_in = perms::group_read,
		group_out = perms::group_write,
		group_exec = perms::group_exe,
		group_all = perms::group_all,
		other_in = perms::others_read,
		other_out = perms::others_write,
		other_exec = perms::others_exe,
		other_all = perms::others_all,
		all_access = perms::all_all,
		no_access = perms::no_perms;
}
#else
#error Cannot find an implementation of std::filesystem.
#endif // file system

#include <fstream>

namespace sys::file
{
	constexpr auto
		owner_io = owner_in | owner_out,
		group_io = group_in | group_out,
		other_io = other_in | other_out,
		all_in = owner_in | group_in | other_in,
		all_out = owner_out | group_out | other_out,
		all_io = owner_io | group_io | other_io;

	inline bool is_executable(path const &exe)
	{
		return no_access != (status(exe).permissions() & group_exec);
	}

	inline bool append(path name, std::string_view text)
	{
		std::ofstream stream(name, std::ios::app);
		return stream and stream << text;
	}

	inline bool truncate(path name)
	{
		std::ofstream stream(name, std::ios::trunc);
		return stream.good();
	}
}

#endif // file
