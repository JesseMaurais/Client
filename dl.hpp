#ifndef dl_hpp
#define dl_hpp

#include "str.hpp"
#include "os.hpp"

#if defined(__WIN32__)
#define dynamic extern "C" __declspec(dllexport)
#else // __POSIX__
#define dynamic extern "C" 
#endif

namespace sys
{
	class dl
	{
		using string_view = fmt::string_view;
		using string = fmt::string;

	public:

		dl(string_view path);
		~dl();
		dl();

		operator bool() const;
		
		template <typename S> auto sym(string_view name) const
		{
			S *addr = nullptr;
			// see pubs.opengroup.org
			*(void**)(&addr) = sym(name);
			return addr;
		}

	private:

		void *tab;
		void *sym(string_view name) const;
	};
}

#endif // file
