#ifndef mem_hpp
#define mem_hpp

#include "file.hpp"
#include "str.hpp"
#include <memory>

namespace sys
{
	struct mem;
}

namespace sys::file
{
	struct memory
	{
		memory(int fd, ssize_t size = -1, size_t offset = 0);
		~memory();

		operator fmt::string_view() const
		{
			auto data = static_cast<char*>(address);
			return fmt::string_view(data, size);
		}

		operator fmt::wstring_view() const
		{
			auto data = static_cast<wchar_t*>(address);
			return fmt::wstring_view(data, size);
		}

	private:
		
		std::unique_ptr<sys::mem> mem;
		void* address =  nullptr;
		size_t size = 0;
	};
}

#endif // file
