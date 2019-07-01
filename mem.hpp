#ifndef mem_hpp
#define mem_hpp

#include "file.hpp"
#include "str.hpp"
#include <memory>

namespace sys::file
{
	struct memory
	{
		enum { none = 0, read = 1, write = 2, execute = 4 };
		enum { share = 1, privy = 2, fixed = 4 };

		memory(int fd, ssize_t size = -1, size_t offset = 0, int mode = read, int flags = share);
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
			auto const addr = reinterpret_cast<Char const*>(data());
			auto const len = size() / sizeof (Char);
			return string_view(addr, len);
		}

		using memory::memory;
	};

	using view = basic_view<char>;
	using wview = basic_view<wchar_t>;
}

#endif // file
