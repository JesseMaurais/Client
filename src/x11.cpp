#include <iostream>
#include <fstream>
#include "fmt.hpp"
#include "dir.hpp"
#include "file.hpp"
#include "x11/auth.hpp"

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
		operator type() const final
		{
			auto u = env::var::get("XAUTHORITY");
			if (empty(u))
			{
				constexpr auto author = ".Xauthority";
				static auto s = fmt::dir::join({ env::home, author });
				u = s;
			}
			return u;
		}

	} XAUTHORITY;
}

namespace x11::auth
{
	bytes::in::ref operator>>(bytes::in::ref in, info::ref out)
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

			for (auto s : ptr)
			{
				if (in.get(b, 2))
				{
					s->resize(sz);
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
	env::view::ref authority = XAUTHORITY;

	fmt::string setup(io::ref str, fmt::string::view proto, fmt::string::view data)
	{
		fmt::string reason;

		if (str) // write
		{
			ClientPrefix client
			{
				.byteOrder = order(),
				.majorVersion = 11,
				.minorVersion = 0,
				.nbytesAuthProto = fmt::to<CARD16>(proto.size()),
				.nbytesAuthString = fmt::to<CARD16>(data.size())
			};

			verify(str << client);
			verify(str.write(proto.data(), client.nbytesAuthProto));
			verify(str.write(data.data(), client.nbytesAuthString));
		}

		if (str) // read
		{
			SetupPrefix prefix;
			if (str >> prefix and Success != prefix.success)
			{
				reason.resize(prefix.lengthReason);
				verify(str.read(data(reason), prefix.lengthReason));
			}
		}

		return reason;
	}
}
#endif
