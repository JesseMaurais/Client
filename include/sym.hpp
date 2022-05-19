#ifndef sym_hpp
#define sym_hpp "Dynamic Link Library"

#include "fmt.hpp"

#ifndef dynamic
#	ifdef _WIN32
#		define dynamic extern "C" __declspec(dllexport)
#	else
#		define dynamic extern "C"
#	endif
#endif

namespace sys
{
	struct lib
	{
		lib() = default;
		lib(fmt::view);
		~lib();

		static lib find(fmt::view);

		template <class Type> auto sym(fmt::view name) const
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
		void *sym(fmt::view) const;
	};

	lib & bin();

	template <class T> auto sym(fmt::view name)
	{
		return bin().sym<T>(name);
	}
}

#endif // file
