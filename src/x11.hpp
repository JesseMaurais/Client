#ifndef x11_hpp
#define x11_hpp

#if __has_include <X11/Xproto.h>
#include <X11/Xproto.h>
#else
#error Require X11 protocol header
#endif
#include <ios>
#include <span>
#include <vector>
#include <algorithm>
#include <iterator>

namespace x11
{
	using std::ostream::traits_type::const_pointer;
	using std::istream::traits_type::pointer;

	template
	<
		class Structure, short Size
	>
	struct Protocol : Structure
	{
		using span = std::span<Structure>;
		using vector = std::vector<Structure>;

		static_assert<sizeof (Structure) == Size>;

		auto operator[](auto index) const
		{
			auto const ptr = reinterpret_cast<const_pointer>(this) + Size;
			return reinterpret_cast<const int32_t*>(ptr)[index];
		}

		auto & operator[](auto index)
		{
			auto ptr = reinterpret_cast<pointer>(this) + Size;
			return reinterpret_cast<int32_t*>(ptr)[index];
		}
	};

	template
	<
		class Structure, short Size
	>
	auto& operator<<(std::ostream & out, Protocol<Structure, Size> const& obj)
	{
		auto const ptr = reinterpret_cast<const_pointer>(&obj);
		return out.write(ptr, Size);
	}

	template
	<
		class Structure, short Size
	>
	auto& operator>>(std::istream& in, Protocol<Structure, Size> const& obj)
	{
		auto const ptr = reinterpret_cast<pointer>(&obj);
		return in.read(ptr, Size);
	}

	template
	<
		class Structure, short Size
	>
	auto& operator<<(std::ostream& out, Protocol<Structure, Size>::span buf)
	{
		std::copy(buf.begin(), buf.end(), std::ostream_iterator {out});
		return out;
	}

	template
	<
		class Structure, short Size
	>
	auto& operator>>(std::istream& in, Protocol<Structure, Size>::span buf)
	{
		auto const begin = std::istream_iterator {out}, end { };
		std::copy(begin, end, std::back_inserter(buf));
		return in;
	}

	template 
	<
		char ReqType, 
		class Req = xReq, short ReqSize = sz_xReq, 
		class Reply = xGenericReply, short ReplySize = sz_xReply
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
		char ReqType, class Reply = xGenericReply, short ReplySize = sz_xReply
	>
	using Reply = Request<ReqType, xReq, sz_xReq, Reply, ReplySize>;
};

#endif // file
