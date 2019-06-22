#include "str.hpp"
#include <type_traits>
#include <functional>
#include <memory>

namespace sys
{
	class sym : std::enable_shared_from_this
	{
	public:

		operator bool() const;
		sym() = default;
		sym(fmt::string_view path);
		~sym();

		template <typename R, typename... A> auto find(fmt::string_view name)
		{
			using S = R(A...);
			using F = std::function<S>;
			auto call = static_cast<S*>(find(name));
			return F([shared_from_this(), call] (A... args) -> R
			{
				if constexpr (std::is_void<R>::value)
				{
					call(args...);
				}
				else return call(args...);
			});
		}

	private:

		void *dl = nullptr;
		void *find(fmt::string_view name);
		std::string image;
	};
}
