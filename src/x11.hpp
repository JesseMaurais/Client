#ifndef x11_hpp
#define x11_hpp "Window Protocol"
#if __has_include(<X11/Xproto.h>)

#include "fmt.hpp"
#include <X11/Xproto.h>

namespace x11
{
	using bytes = fmt::string;

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

		friend bytes::out::ref operator<<(bytes::out::ref out, cref obj)
		{
			auto const ptr = reinterpret_cast<bytes::const_pointer>(&obj);
			return out.write(ptr, Size);
		}

		friend bytes::in::ref operator>>(bytes::in::ref in, ref obj)
		{
			auto const ptr = reinterpret_cast<bytes::pointer>(&obj);
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
