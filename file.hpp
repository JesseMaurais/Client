#ifndef file_hpp
#define file_hpp

#include <ios>
#include <utility>
#include <string>
#include <string_view>
#include <initializer_list>

namespace sys::file
{
	using size_t = std::size_t;
	using ssize_t = std::make_signed<size_t>::type;
	using openmode = std::ios_base::openmode;

	constexpr auto app   = std::ios_base::app;
	constexpr auto bin   = std::ios_base::binary;
	constexpr auto in    = std::ios_base::in;
	constexpr auto out   = std::ios_base::out;
	constexpr auto trunc = std::ios_base::trunc;
	constexpr auto ate   = std::ios_base::ate;

	struct descriptor
	{
		void open(std::string const& path, openmode mode);
		ssize_t write(const void* buffer, size_t size);
		ssize_t read(void* buffer, size_t size);
		~descriptor();

		descriptor(int fd = -1)
		{
			this->fd = fd;
		}

		int get() const
		{
			return fd;
		}

		int set(int fd)
		{
			std::swap(fd, this->fd);
			return fd;
		}

	protected:

		int fd;
	};

	struct pipe
	{
		pipe();

		ssize_t write(const void* buffer, size_t size)
		{
			return fd[1].write(buffer, size);
		}

		ssize_t read(void* buffer, size_t size)
		{
			return fd[0].read(buffer, size);
		}

		descriptor& operator[](size_t index)
		{
			return fd[index];
		}

		operator bool() const
		{
			return fd[0].get() == -1 and fd[1].get() == -1;
		}

	protected:

		descriptor fd[2];
	};

	struct process : pipe
	{
		using arguments = std::initializer_list<std::string_view>;

		void open(arguments args, openmode mode);

	protected:

		std::intptr_t pid;
	};
}

#endif // file