#ifndef file_hpp
#define file_hpp

#include <memory>
#include <utility>
#include <functional>
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

	constexpr auto append   = std::ios_base::app;
	constexpr auto binary   = std::ios_base::binary;
	constexpr auto in       = std::ios_base::in;
	constexpr auto out      = std::ios_base::out;
	constexpr auto truncate = std::ios_base::trunc;
	constexpr auto end      = std::ios_base::ate;

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
			close();
		}

		operator bool() const
		{
			return fail(fd);
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

		operator bool() const
		{
			return !!file[0] and !!file[1];
		}

		void set(int fd[2] = nullptr)
		{
			for (int i : { 0, 1 })
			{
				file[i].set(fd ? fd[i] : -1);
			}
		}

		ssize_t write(const void* buffer, size_t size)
		{
			return file[1].write(buffer, size);
		}

		ssize_t read(void* buffer, size_t size)
		{
			return file[0].read(buffer, size);
		}

	protected:

		descriptor file[2];
	};

	struct process : pipe
	{
		using arguments = std::initializer_list<char const *>;

		bool open(arguments, openmode);

	protected:

		descriptor error;
		std::intptr_t pid;
	};
}

#endif // file