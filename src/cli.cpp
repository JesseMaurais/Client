#include "cli.hpp"
#include "env.hpp"
#include "dir.hpp"
#include "err.hpp"
#include "ips.hpp"

namespace env::cli
{
	bool const cmd = sys::got("ComSpec");
	bool const sh = sys::got("SHELL");

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
			return { "findstr", pattern };
		} 
		else 
		if (sh)
		{
			return { "grep", pattern };
		}
		else
		{
			return { };
		}
	}
/*	
	void where(fmt::view program, entry peek)
	{
		auto pin = cli::where(program);
		auto 
		if (in)
		{
			fmt::string s;
			io::pstream in(cmd);
			while (std::getline(in, s))
			{
				if (peek(s))
				{
					break;
				}
			}
		}
		else
		{
			using namespace env::dir;
			using namespace ::sys::file;
			auto f = mask(ex) || regx(program) || peek;
			fmt::span<fmt::view> const paths = env::path;
			for (auto const p : paths)
			{
				if (find(p, f))
				{
					break;
				}
			}
		}
	}
	*/
}

