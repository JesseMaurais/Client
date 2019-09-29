#include "cli.hpp"
#include "env.hpp"
#include "dir.hpp"
#include "err.hpp"
#include "buf.hpp"
#include "direct.hpp"

namespace env::cli
{
	bool const cmd = var::got("ComSpec");
	bool const sh = var::got("SHELL");

	process where(fmt::view program)
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

	process find(fmt::view pattern)
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
}

