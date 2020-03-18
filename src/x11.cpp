#include <iostream>
#include <fstream>
#include "fmt.hpp"
#include "dir.hpp"
#include "file.hpp"
#include "x11/auth.hpp"
#include "x11/client.hpp"

namespace
{
	char order()
	{
		union 
		{
		 char c[2];
		 short s; 
		};
		s = 1;
		return *c ? 'l' : 'B';
	}

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

namespace x11::auth
{
	std::istream& operator>>(std::istream& in, info& out)
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

namespace x11
{
	env::view const& authority = XAUTHORITY;

	fmt::string setup(std::iostream& io, fmt::string_view proto, fmt::string_view string)
	{
		fmt::string reason;

		if (io) // write
		{
			ClientPrefix client
			{
				.byteOrder = order(),
				.majorVersion = 11,
				.minorVersion = 0,
				.nbytesAuthProto = fmt::to<CARD16>(proto.size()),
				.nbytesAuthString = fmt::to<CARD16>(string.size())
			};

			verify(io << client);
			verify(io.write(proto.data(), client.nbytesAuthProto));
			verify(io.write(string.data(), client.nbytesAuthString));
		}

		if (io) // read
		{
			SetupPrefix prefix;
			if (io >> prefix and Success != prefix.success)
			{
				reason.resize(prefix.lengthReason);
				verify(io.read(data(reason), prefix.lengthReason));
			}
		}

		return reason;
	}
}
#endif
