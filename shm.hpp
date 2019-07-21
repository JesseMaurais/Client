#ifndef shm_hpp
#define shm_hpp

#include "file.hpp"
#include "str.hpp"
#include <memory>

namespace sys::file
{
	struct memory
	{
		enum { none = 0, read = 1, write = 2, run = 4 };
		enum { share = 1, privy = 2, fixed = 4 };

		bool open(fmt::string_view name, ssize_t size = -1, size_t off = 0, int mode = read | write, int type = share) 
		bool open(int fd, ssize_t size = -1, size_t off = 0, int mode = read | write, int type = share);
		void close();

		~memory()
		{
			close();
		}

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

		fmt::string_view name() const
		{
			return path;
		}

	private:

		void* address = nullptr;
		size_t length = 0;
		fmt::string path;
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
