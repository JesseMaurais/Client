#include "run.hpp"
#include "env.hpp"
#include "dir.hpp"
#include "buf.hpp"

namespace env::run
{
	bool const cmd = ::env::var::got("ComSpec");
	bool const sh = ::env::var::got("SHELL");

	static process where(view program)
	{
		if (cmd)
		{
			return process { "where", program };
		}
		else 
		if (sh)
		{
			return process { "which", program };
		}
		else
		{
			return { };
		}
	}

	static process find(view pattern)
	{
		if (cmd)
		{
			return process { "findstr", pattern };
		} 
		else 
		if (sh)
		{
			return process { "grep", pattern };
		}
		else
		{
			return { };
		}
	}

	bool getline(process& go, string& s)
	{
		io::rstream in(go);
		return !!std::getline(in, s);
	}

	bool where(view program, entry look)
	{
		auto go = where(program);
		auto const id = go.get();
		if (fail(id))
		{
			using namespace dir;
			return path(ex || regx(program) || look);
		}
		return with(go, look);
	}

	bool find(fmt::view pattern, entry look)
	{
		auto go = find(pattern);
		return with(go, look);
	}
}

