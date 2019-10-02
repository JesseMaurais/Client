#ifndef mode_hpp
#define mode_hpp

#include <cstddef>
#include <cstdint>
#include "str.hpp"
#include "ptr.hpp"
#include "tmp.hpp"

namespace env::file
{
	constexpr int invalid = -1;

	inline bool fail(int value)
	{
		return invalid == value;
	}

	// Access modes
	enum mode : int
	{
		ex   = 1 << 0, // execute
		wr   = 1 << 1, // write
		rd   = 1 << 2, // read
		ok   = 1 << 3, // exists
		xu   = 1 << 4, // exclusive
		sz   = 1 << 5, // truncate
		app  = 1 << 6, // append
		txt  = 1 << 7, // text
		bin  = 1 << 8, // binary
		rw   = rd | wr,
		wo   = rw | ok,
		rwx  = rw | ex,
		oz   = wo | sz,
		ox   = wo | ex,
	};

	constexpr int owner(int am)
	{
		return (am & rwx) << 6;
	}

	constexpr int group(int am)
	{
		return (am & rwx) << 3;
	}

	constexpr int other(int am)
	{
		return (am & rwx) << 0;
	}

	// Permissions
	enum permit : int
	{
		owner_r = owner(rd),
		owner_w = owner(wr),
		owner_x = owner(ex),
		group_r = group(rd),
		group_w = group(wr),
		group_x = group(ex),
		other_r = other(rd),
		other_w = other(wr),
		other_x = other(ex),
	};

	inline permit owner(mode am)
	{
		return static_cast<permit>(owner(static_cast<int>(am)));
	}

	inline permit group(mode am)
	{
		return static_cast<permit>(group(static_cast<int>(am)));
	}

	inline permit other(mode am)
	{
		return static_cast<permit>(other(static_cast<int>(am)));
	}

	int check(mode); // file access mode
	int convert(mode); // file open mode
	int convert(permit); // permissions

	// Check for access to the file at path
	bool fail(fmt::view path, mode = ok);

	// Adjustable file buffer size
	extern attribute<size_t>& width;
}

#endif // file
