#ifndef file_hpp
#define file_hpp

#include <utility>
#include <string>
#include <ios>

namespace sys
{
	template <typename I> constexpr bool fail(I const value)
	{
		constexpr I invalid = -1;
		return invalid == value;
	}
}

namespace sys::file
{
	using sys::fail;
	using size_t = std::size_t;
	using ssize_t = std::make_signed<size_t>::type;
	using openmode = std::ios_base::openmode;

	extern size_t bufsiz;

	constexpr auto app  = std::ios_base::app;
	constexpr auto bin  = std::ios_base::binary;
	constexpr auto in   = std::ios_base::in;
	constexpr auto out  = std::ios_base::out;
	constexpr auto trun = std::ios_base::trunc;
	constexpr auto ate  = std::ios_base::ate;

	struct descriptor
	{
		void open(std::string const& path, openmode mode);
		ssize_t write(const void* buffer, size_t size);
		ssize_t read(void* buffer, size_t size);
		bool close();

		explicit descriptor(int fd = -1)
		{
			this->fd = fd;
		}

		~descriptor()
		{
			(void) close();
		}

		int get() const
		{
			return fd;
		}

		int set(int fd = -1)
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
			return fail(fd[0].get()) and fail(fd[1].get());
		}

	protected:

		descriptor fd[2];
	};

	struct process : pipe
	{
		using arguments = std::initializer_list<char const *>;

		void open(arguments args, openmode mode);

	protected:

		descriptor error;
		std::intptr_t pid;
	};
}

#endif // file
