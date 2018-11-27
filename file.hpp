#ifndef file_hpp
#define file_hpp

#include <ios>
#include <string>
#include <initializer_list>

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

		void open(std::string const& path, openmode mode);
		~descriptor();

		descriptor(int fd = -1)
		{
			this->fd = fd;
		}

		operator int() const
		{
			return fd;
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

		void open(std::initializer_list<char*> args, openmode mode);
		pipe();

		pipe(int fd0, int fd1)
		{
			this->fd[0] = fd0;
			this->fd[1] = fd1;
		}

		operator bool() const
		{
			return -1 != fd[0] or -1 != fd[1];
		}

		int operator[](std::size_t id) const
		{
			return id < 2 ? (int) fd[id] : -1;
		}

		int release(std::size_t id)
		{
			return id < 2 ? fd[id].release() : -1;
		}

	private:

		descriptor fd[2];
	};
}

#endif // file