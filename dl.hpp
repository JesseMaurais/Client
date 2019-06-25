#include "str.hpp"

namespace sys
{
	class dl
	{
		using string_view = fmt::string_view;
		using string = fmt::string;

	public:

		dl();
		dl(string_view path);
		~dl();

		operator bool() const;
		
		template <typename S> auto sym(string_view name) const
		{
			S *addr = nullptr;
			// see pubs.opengroup.org
			*(void**)(&addr) = sym(name);
			return addr;
		}

	private:

		void *tab = nullptr;
		void *sym(string_view name) const;
	};
}

