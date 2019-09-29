#include "run.hpp"
#include "env.hpp"
#include "cli.hpp"
#include "dir.hpp"
#include "buf.hpp"

namespace env::run
{
	bool const cmd = ::env::var::got("ComSpec");
	bool const sh = ::env::var::got("SHELL");

	bool getline(process& go, string& s)
	{
		io::rstream in(go);
		return !!std::getline(in, s);
	}

	bool where(view program, entry look)
	{
		auto go = cli::where(program);
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
		auto go = cli::find(pattern);
		return with(go, look);
	}
}

