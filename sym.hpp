#include "str.hpp"
#include <functional>

namespace sys
{
	class sym
	{
		using string_view = fmt::string_view;
		using string = fmt::string;

	public:

		operator bool() const;
		sym() = default;
		sym(string_view path);
		~sym();

		template <typename S> auto link(string_view name)
		{
			S *addr = nullptr;
			// see pubs.opengroup.org
			*(void**)(&addr) = link(name);
			return std::function<S>(addr);
		}

	private:

		void *dl = nullptr;
		void *link(string_view name);
	};
}
