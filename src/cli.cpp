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

	void where(xpath program, entry peek)
	{
		auto const line = cli::where(program);
		auto const id = line.get();
		if (not fail(id))
		{
			std::string s;
			io::pstream in(line);
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
			auto f = ex || regx(program) || peek;
			if (find(env::pwd, f))
			{
				return;
			}
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
}

