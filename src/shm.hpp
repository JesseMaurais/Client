#ifndef shm_hpp
#define shm_hpp

#include "file.hpp"
#include "str.hpp"

namespace sys::file
{
	struct memory
	{
		enum { none = 0, read = 1, write = 2, run = 4 };
		enum { share = 1, privy = 2, fixed = 4 };

		bool open(char const* name, ssize_t sz = -1, size_t off = 0, int mode = read, int type = share, void* ptr = nullptr)
		{
			return open(name, -1, sz, off, mode, type, ptr);
		}

		bool open(int fd, ssize_t sz = -1, size_t off = 0, int mode = read, int type = share, void* ptr = nullptr)
		{
			return open(nullptr, fd, sz, off, mode, type, ptr);
		}

		bool close();

		~memory()
		{
			(void) close();
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

		bool open(char const* name, int fd, ssize_t sz, size_t off, int mode, int type, void* ptr);

		void* address = nullptr;
		size_t length = 0;
		fmt::string path;
	};

	template
	<
	 typename Char,
	 template <typename> class Traits = std::char_traits
	>
	class basic_view : public memory
	{
		using string_view = fmt::basic_string_view<Char, Traits<Char>>;

	public:

		operator string_view() const
		{
			auto const ptr = reinterpret_cast<Char const*>(data());
			auto const sz = size() / sizeof (Char);
			return string_view(ptr, sz);
		}
	};

	using view = basic_view<char>;
	using wview = basic_view<wchar_t>;
}

#endif // file
