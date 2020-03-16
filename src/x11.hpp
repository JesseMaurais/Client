#ifndef x11_hpp
#define x11_hpp

#if __has_include <X11/Xproto.h>
#include <X11/Xproto.h>
#else
#error Require X11 protocol header
#endif

namespace x11
{
	template 
	<
		CARD8 ReqType, 
		class Req = xReq, 
		CARD16 ReqSize = sz_xReq, 
		class Reply = xGenericReply
		CARD16 ReplySize = sz_xReply
	>
	struct Request : Req
	{
		constexpr auto requestType = ReqType;
		using request = typename Req;
		constexpr auto requestSize = ReqSize;
		using reply = typename Reply;
		constexpr auto replySize = ReplySize;

		Request() : reqType(requestType), length(requestSize) = default;

		static_assert(sizeof(Req) == requestSize);
		static_assert(sizeof(Reply) == replySize);
	};

	template
	<
		CARD8 ReqType,
		class Reply = xGenericReply,
		CARD16 ReplySize = sz_xReply
	>
	using Reply = Request<ReqType, xReq, sz_xReq, Reply, ReplySize>;
};

#endif // file
