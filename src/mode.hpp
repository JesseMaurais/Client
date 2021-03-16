#ifndef mode_hpp
#define mode_hpp "Access Mode"

#include <cstddef>
#include "fmt.hpp"
#include "tmp.hpp"

namespace env::file
{
	// Access modes
	enum mode : int
	{
		ex   = 1 << 000, // execute
		wr   = 1 << 001, // write
		rd   = 1 << 002, // read
		ok   = 1 << 003, // exists
		xu   = 1 << 004, // exclusive
		sz   = 1 << 005, // truncate
		app  = 1 << 006, // append
		txt  = 1 << 007, // text
		bin  = 1 << 010, // binary
		blk  = 1 << 011, // block device
		chr  = 1 << 012, // character device
		dir  = 1 << 013, // directory
		fifo = 1 << 014, // byte stream
		lnk  = 1 << 015, // symbolic link
		reg  = 1 << 016, // regulare file
		sock = 1 << 017, // domain socket

		rw   = rd | wr, // read & write
		wo   = rw | ok, // write & exists
		rwx  = rw | ex, // read, write & execute
		ov   = wo | sz, // overwrite
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
	bool fail(fmt::string::view path, mode = ok);

	// Adjustable file buffer size
	extern fwd::variable<size_t>& width;
}

#endif // file
