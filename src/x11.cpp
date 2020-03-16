#include <fstream>
#include "fmt.hpp"
#include "dir.hpp"
#include "file.hpp"
#include "x11/auth.hpp"
#include "x11/client.hpp"

namespace
{
	struct : env::view
	{
		operator fmt::string:view() const final
		{
			auto const u = env::var::get("XAUTHORITY");
			if (empty(u))
			{
				fmt::vector_view const path { env::home, ".Xauthority" };
				static auto s = fmt::dir::join(path);
				u = s;
			}
			return u;
		}

	} XAUTHORITY;
}

namespace x11
{
	env::view const& authority = XAUTHORITY;
}

namespace x11::auth
{
	std::istream& get(std::istream& in, info& out)
	{
		union {
			unsigned char b[2];
			unsigned short sz;
		};

		if (in.get(b, 2))
		{
			out.family = sz;
		
			auto ptr = 
			{ 
				&out.address, 
				&out.number,
				&out.name,
				&out.data
			};

			for (auto str : ptr)
			{
				if (in.get(b, 2))
				{
					str->resize(sz);
					if (in.get(ptr->data(), sz))
					{
						continue;
					}
				}
				break;
			}
		}
		return in;
	}
};

namespace
{
	char order()
	{
		union {
			short s;
			char c;
		};
		s = 'l' << 8 | 'B';
		return c;
	}
}

namespace x11
{
	fmt::string socket(int screen)
	{
		auto const n = fmt::to_string(screen);
		auto const s = fmt::to_string("X") + n;
		return fmt::dir::join(env::tempdir, ".X11-unix", s);
		#endif
	}

	int open(fmt::string_view name)
	{
		auth::info info;
		std::ifstream in(authority);
		while (auth::get(in, info))
		{
			
		}
		return env::file::invalid;
	}
}

#endif
