#ifndef x11_hpp
#define x11_hpp "Window Protocol"

#include "fmt.hpp"
#if __has_include(<X11/Xproto.h>)
#include <X11/Xproto.h>
#else
#error Require X11 protocol header
#endif

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
		using in = bytes::in;
		using out = bytes::out;

		static_assert(sizeof (Structure) == Size);

		friend out::ref operator<<(out::ref, cref);
		friend in::ref operator>>(in::ref, ref);
		friend out::ref operator<<(out::ref, span);
		friend in::ref operator>>(in::ref, span);
	};

	template
	<
		class Structure, unsigned short Size
	>
	bytes::out::ref operator<<(bytes::out::ref out, typename Protocol<Structure, Size>::cref obj)
	{
		auto const ptr = reinterpret_cast<char const *>(&obj);
		return out.write(ptr, Size);
	}

	template
	<
		class Structure, unsigned short Size
	>
	bytes::in::ref operator>>(bytes::in::ref out, typename Protocol<Structure, Size>::ref obj)
	{
		auto const ptr = reinterpret_cast<char *>(&obj);
		return out.read(ptr, Size);
	}

	template
	<
		class Structure, unsigned short Size
	>
	bytes::out::ref operator<<(bytes::out::ref out, typename Protocol<Structure, Size>::span buf)
	{
		std::copy(buf.begin(), buf.end(), std::ostream_iterator<Structure> {out});
		return out;
	}

	template
	<
		class Structure, unsigned short Size
	>
	bytes::in::ref operator>>(bytes::in::ref in, typename Protocol<Structure, Size>::span buf)
	{
		std::istream_iterator<Structure> const begin {in}, end { };
		std::copy_n(begin, end, buf.size(), buf.data());
		return in;
	}

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

#endif // file
