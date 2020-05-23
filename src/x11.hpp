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
	using bytes = fmt::basic_string<std::byte>;

	template
	<
		class Structure, unsigned short Size
	>
	struct Protocol : Structure
	{
		using span = std::span<Structure>;
		using vector = std::vector<Structure>;
		using cptr = typename vector::const_pointer;
		using ptr = typename vector::pointer;
		using cref = typename vector::const_reference;
		using ref = typename vector::reference;

		static_assert(sizeof (Structure) == Size);
	};

	template
	<
		class Structure, unsigned short Size
	>
	bytes::out::ref operator<<(bytes::out::ref out, Protocol<Structure, Size>::cref obj)
	{
		auto const ptr = reinterpret_cast<const_pointer>(&obj);
		return out.write(ptr, Size);
	}

	template
	<
		class Structure, unsigned short Size
	>
	bytes::in::ref operator>>(bytes::in::ref obj, Protocol<Structure, Size>::ref obj)
	{
		auto const ptr = reinterpret_cast<pointer>(&obj);
		return obj.read(ptr, Size);
	}

	template
	<
		class Structure, unsigned short Size
	>
	bytes::out::ref operator<<(bytes::out::ref out, Protocol<Structure, Size>::span buf)
	{
		std::copy(buf.begin(), buf.end(), std::ostream_iterator {out});
		return out;
	}

	template
	<
		class Structure, unsigned short Size
	>
	byte::in::ref operator>>(bytes::in::ref in, Protocol<Structure, Size>::span buf)
	{
		auto const begin = std::istream_iterator {in}, end { };
		std::copy(begin, end, std::back_inserter(buf));
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
		constexpr auto requestType = ReqType;
		using request = typename Req;
		constexpr auto requestSize = ReqSize;
		using reply = typename Reply;
		constexpr auto replySize = ReplySize;

		static_assert(sizeof (Req) == requestSize);
		static_assert(sizeof (Reply) == replySize);

		Request() : reqType(requestType), length(requestSize) = default;
	};

	template
	<
		char ReqType, class Reply = xGenericReply, unsigned short ReplySize = sz_xReply
	>
	using Reply = Request<ReqType, xReq, sz_xReq, Reply, ReplySize>;
};

#endif // file
