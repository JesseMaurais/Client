// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#if !__has_include(<X11/Xproto.h>)
# error Cannot find X11 protocol header
#endif

#include <iostream>
#include <fstream>
#include "fmt.hpp"
#include "env.hpp"
#include "dig.hpp"
#include "dir.hpp"
#include "file.hpp"
#include "x11/auth.hpp"
#include "x11/setup.hpp"

namespace
{
	char byte_order()
	{
		union
		{
			char c[2];
			short s;
		};
		s = 1;
		return *c ? 'l' : 'B';
	}
}

namespace x11::auth
{
	fmt::input operator>>(fmt::input in, info::ref out)
	{
		union {
			char b[2];
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
					if (in.get(s->data(), sz))
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
	fmt::view authority()
	{
		auto u = env::get("XAUTHORITY");
		if (u.empty())
		{
			static fmt::string s;
			if (s.empty())
			{
				constexpr auto author = ".Xauthority";
				const auto home = env::home();
				s = fmt::dir::join({ home, author });
			}
			u = s;
		}
		return u;
	}

	fmt::string setup(fmt::binary io, fmt::view proto, fmt::view data)
	{
		fmt::string reason;

		if (io) // write
		{
			ClientPrefix client;
			client.byteOrder = byte_order();
			client.majorVersion = 11;
			client.minorVersion = 0;
			client.nbytesAuthProto = fmt::to<CARD16>(proto.size());
			client.nbytesAuthString = fmt::to<CARD16>(data.size());

			io << client;
			io.write(proto.data(), client.nbytesAuthProto);
			io.write(data.data(), client.nbytesAuthString);
		}

		if (io) // read
		{
			SetupPrefix prefix;
			if (io >> prefix and not prefix.success)
			{
				reason.resize(prefix.lengthReason);
				io.read(reason.data(), prefix.lengthReason);
			}
		}

		return reason;
	}
}
