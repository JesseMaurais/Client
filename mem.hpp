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

		void* data()
		{
			return address;
		}

		const void* data() const
		{
			return address;
		}

		size_t size() const
		{
			return length;
		}

	private:
		
		std::unique_ptr<sys::mem> mem;
		void* address = nullptr;
		size_t length = 0;
	};

	template
	<
	 typename Char,
	 template <typename> class Traits = std::char_traits
	>
	class basic_view : memory
	{
		using string_view = fmt::basic_string_view<Char, Traits<Char>>;

	public:

		operator string_view() const
		{
			auto const address = reinterpret_cast<Char const*>(data());
			auto const length = size() / sizeof (Char);
			return string_view(address, length);
		}

		using memory::memory;
	};

	using view = basic_view<char>;
	using wview = basic_view<wchar_t>;
}

#endif // file
