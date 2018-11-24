#ifndef tool_hpp
#define tool_hpp

#include <utility>
#include <array>

namespace stl
{
	struct not_copyable
	{	
		not_copyable() = default;
		not_copyable &operator=(const not_copyable &) = delete;
		not_copyable(const not_copyable &) = delete;
	};

	namespace impl
	{
		template <typename T, size_t N, size_t... i, size_t M, size_t... j>
		constexpr std::array<T, N+M> cat(T const (&a)[N], T const (&b)[M],
			 std::index_sequence<i...>, std::index_sequence<j...>)
		{
			return std::array<T, N+M>{ a[i]..., b[j]... };
		}
	}

	template <typename T, size_t N, size_t M>
	constexpr std::array<T, N+M> cat(const T (&a)[N], const T (&b)[M])
	{
		return impl::cat(a, b, std::make_index_sequence<N>{}, std::make_index_sequence<M>{});
	}
}

#endif // file
