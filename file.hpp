#ifndef file_hpp
#define file_hpp

#include <ios>
#include <string_view>

namespace sys::file
{
	using openmode = std::ios_base::openmode;

	constexpr auto app   = std::ios_base::app;
	constexpr auto bin   = std::ios_base::binary;
	constexpr auto in    = std::ios_base::in;
	constexpr auto out   = std::ios_base::out;
	constexpr auto trunc = std::ios_base::trunc;
	constexpr auto ate   = std::ios_base::ate;

	class descriptor
	{
	public:

		void open(std::string_view view, openmode mode);

		~descriptor();
		descriptor(int fd = -1)
		{
			this->fd = fd;
		}

		operator int() const
		{
			return fd;
		}

		operator bool() const
		{
			return -1 != fd;
		}

		int release()
		{
			int fd = this->fd;
			this->fd = -1;
			return fd;
		}

	private:

		int fd;
	};

	class pipe
	{
	public:

		pipe();

		int operator[](std::size_t id) const
		{
			return id < 2 ? (int) fds[id] : -1;
		}

		operator bool() const
		{
			return fds[0] or fds[1];
		}

		int release(std::size_t id)
		{
			return id < 2 ? fds[id].release() : -1;
		}

	private:

		descriptor fds[2];
	};
}

#endif // file
