#ifndef sym_hpp
#define sym_hpp "Dynamic Link Library"

#include "fmt.hpp"
#include "env.hpp"

#ifndef dynamic
#	ifdef _WIN32
#		define dynamic extern "C" __declspec(dllexport)
#	else
#		define dynamic extern "C" 
#	endif
#else
#	warning Using linkage dynamic // permitted
#endif

namespace sys
{
	extern env::view::ref thread_id;

	struct dll : fmt::struct_brief<dll>
	{
		dll() = default;
		dll(fmt::string::view);
		~dll();

		static dll find(fmt::string::view);
	
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

	extern dll::cref self;

	template <class T> auto sym(fmt::string::view name)
	{
		return self.sym<T>(name);
	}
}

#endif // file
