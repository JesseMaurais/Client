#ifndef dll_hpp
#define dll_hpp "Dynamic Link Library"

#include "fmt.hpp"

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
		dll(fmt::string::view path);
		~dll();

		static dll find(fmt::string::view name);
	
		template 
		<
			class Type
		> 
		auto sym(fmt::string::view name) const
		{
			Type *addr = nullptr;
			// see pubs.opengroup.org
			*(void**)(&addr) = sym(name);
			return addr;
		}

		operator bool() const
		{
			return nullptr != ptr;
		}

	private:

		void *ptr = nullptr;
		void *sym(fmt::string::view) const;
	};
}

#endif // file
