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

namespace x11
{
	using std::ostream::traits_type::const_pointer;
	using std::istream::traits_type::pointer;

	std::ostream& operator<<(std::ostream& out, const xReq& request)
	{
		auto const ptr = reinterpret_cast<const_pointer>(&xReq);
		return out.write(ptr, request.length);
	}

	std::istream& operator>>(std::istream& in, xReply& reply) 
	{
		auto ptr = reinterpret_cast<pointer>(&reply);
		return in.read(ptr, request.length);
	}

	fmt::string setup(std::iostream& io, fmt::string_view proto, fmt::string_view author)
	{
		fmt::string reason;

		if (io) // write
		{
			xConnClientPrefix client
			{
			.byteOrder = order(),
			.majorVersion = 11,
			.minorVersion = 0,
			.nbytesAuthProto = fmt::to<CARD16>(size(proto)),
			.nbytesAuthString = fmt::to<CARD16>(size(author))
			};

			auto const ptr = reinterpret_cast<const_pointer>(&client);
			verify(io.write(ptr, sz_xConnClientPrefix));
			verify(io.write(data(proto), client.nbytesAuthProto));
			verify(io.write(data(author), client.nbytesAuthString));
		}

		if (io) // read
		{
			xConnSetupPrefix prefix;
			auto ptr = reinterpret_cast<pointer>(&prefix);
			verify(io.read(ptr, sz_xConnSetupPrefix));
			if (io and not prefix.success)
			{
				reason.resize(prefix.lengthReason);
				verify(io.read(data(reason), prefix.lengthReason));
			}
		}

		return reason;
	}

	env::view const& authority = XAUTHORITY;
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

#endif
