// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "dev.hpp"
#include "ps.hpp"
#include "type.hpp"

namespace env::dev
{
	shell::line dump::sym(view path)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "dumpbin", "-nologo", "-imports", path };
		#else
			{ "objdump", "-t", path };
		#endif
		return get(sub);
	}

	shell::line dump::dyn(view path)
	{
		fmt::ipstream sub
		#ifdef _WIN32
			{ "dumpbin", "-nologo", "-exports", path };
		#else
			{ "objdump", "-T", path };
		#endif
		return get(sub);
	}
};

