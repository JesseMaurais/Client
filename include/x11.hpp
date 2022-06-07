#ifndef x11_hpp
#define x11_hpp "Window Protocol"
#if __has_include(<X11/Xproto.h>)

#include "fmt.hpp"
#include <X11/Xproto.h>

namespace x11
{
	template
	<
		class Structure, unsigned short Size
	>
	struct Protocol : Structure
	{
		using span = std::span<Structure>;
		using vector = std::vector<Structure>;
		using cptr = typename span::const_pointer;
		using ptr = typename span::pointer;
		using cref = typename span::const_reference;
		using ref = typename span::reference;

		friend fmt::output operator<<(fmt::output out, cref obj)
		{
			auto ptr = fwd::cast_as<const char>>(&obj);
			return out.write(ptr, Size);
		}

		friend fmt::input operator>>(fmt::input in, ref obj)
		{
			auto ptr = fwd::cast_as<char>(&obj);
			return in.read(ptr, Size);
		}
	};

	template
	<
		char ReqType,
		class Req = xReq, unsigned short ReqSize = sz_xReq,
		class Reply = xGenericReply, unsigned short ReplySize = sz_xReply
	>
	struct Request : Protocol<Req, ReqSize>
	{
		static constexpr auto requestType = ReqType;
		using request = Req;
		static constexpr auto requestSize = ReqSize;
		using reply = Reply;
		static constexpr auto replySize = ReplySize;

		static_assert(sizeof (Req) == requestSize);
		static_assert(sizeof (Reply) == replySize);

		Request()
		: Req::reqType(requestType), Req::length(requestSize)
		{ }
	};

	template
	<
		char ReqType, class Reply = xGenericReply, unsigned short ReplySize = sz_xReply
	>
	using Reply = Request<ReqType, xReq, sz_xReq, Reply, ReplySize>;
};

#endif // Xproto
#endif // file
