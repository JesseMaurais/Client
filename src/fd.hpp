#ifndef fd_hpp
#define fd_hpp

#include "type.hpp"
#include "ptr.hpp"
#include "file.hpp"
#include "mode.hpp"

namespace env::file
{
	struct descriptor : unique, stream
	{
		ssize_t read(void *buf, size_t sz) const override;
		ssize_t write(const void *buf, size_t sz) const override;
		bool open(fmt::string::view path, mode = rw, permit = owner(rw));
		bool close();

		explicit descriptor(fmt::string_view path, mode am = rw, permit pm = owner(rw))
		{
			(void) open(path, am, pm);
		}

		explicit descriptor(int fd = invalid)
		{
			(void) set(fd);
		}

		~descriptor()
		{
			if (not fail(fd))
			{
				(void) close();
			}
		}

		int set(int value = invalid)
		{
			int const tmp = fd;
			fd = value;
			return tmp;
		}

		int get() const
		{
			return fd;
		}

	protected:

		int fd;
	};
}

#endif // file
