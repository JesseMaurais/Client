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

		dll() = default;
		dll(fmt::string_view path);
		~dll();

		dll find(fmt::string_view name);
	
		template <typename S> auto sym(fmt::string_view name) const
		{
			S *addr = nullptr;
			// see pubs.opengroup.org
			*(void**)(&addr) = sym(name);
			return addr;
		}

		operator bool() const
		{
			return nullptr == ptr;
		}

	private:

		void *ptr = nullptr;
		void *sym(fmt::string_view name) const;
	};
}

#endif // file
