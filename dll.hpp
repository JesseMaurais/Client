#ifndef dll_hpp
#define dll_hpp

#include "str.hpp"

#ifdef _WIN32
#define dynamic extern "C" __declspec(dllexport)
#else
#define dynamic extern "C" 
#endif

namespace sys
{
	class dll
	{
	public:

		dll(fmt::string_view path);
		~dll();
		dll();

		operator bool() const;

		static dll find(fmt::string_view name);
		
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
