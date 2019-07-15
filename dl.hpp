#ifndef dl_hpp
#define dl_hpp

#include "str.hpp"

#ifdef _WIN32
#define dynamic extern "C" __declspec(dllexport)
#else
#define dynamic extern "C" 
#endif

namespace sys
{
	class dl
	{
	public:

		dl(fmt::string_view path);
		~dl();
		dl();

		operator bool() const;

		static dl find(fmt::string_view name);
		
		template <typename S> auto sym(fmt::string_view name) const
		{
			S *addr = nullptr;
			// see pubs.opengroup.org
			*(void**)(&addr) = sym(name);
			return addr;
		}

	private:

		void *tab;
		void *sym(fmt::string_view name) const;
	};
}

#endif // file
