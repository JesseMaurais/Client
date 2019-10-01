#ifndef dir_hpp
#define dir_hpp

#include "str.hpp"
#include "tmp.hpp"
#include "file.hpp"

namespace fmt::path
{
	list_view split(view);
	string join(span_view);
}

namespace fmt::dir
{
	list_view split(view);
	string join(span_view);
}

namespace env::dir
{
	using entry = predicate<fmt::view>;

	entry mask(file::mode);
	entry regx(fmt::view);
	entry to(fmt::string &);
	entry to(fmt::string_list &);

	constexpr auto stop = always<fmt::view>;
	constexpr auto skip = never<fmt::view>;

	static auto // access
		ex = mask(file::ex),
		wr = mask(file::wr),
		rd = mask(file::rd),
		ok = mask(file::ok),
		rw = mask(file::rw),
		rwx = mask(file::rwx);

	inline auto all(fmt::view u, file::mode m = file::ok, entry e = skip)
	{
		return mask(m) || regx(u) || e;
	}

	inline auto any(fmt::view u, file::mode m = file::ok)
	{
		return all(u, m, stop);
	}

	extern env::pair const& paths;
	extern env::pair const& config;
	extern env::pair const& data;

	bool find(fmt::view, entry);
	bool find(fmt::span_view, entry);
	bool find(fmt::span_pair, entry);

	bool fail(fmt::view path, file::mode = file::ok);
	fmt::view make(fmt::view path);
	bool remove(fmt::view path);

	class tmp
	{
		fmt::view stem;

	public:

		tmp(fmt::view path)
		{
			stem = env::dir::make(path);
		}

		~tmp()
		{
			if (not empty(stem))
			{
				env::dir::remove(stem);
			}
		}
	};
}

#endif // file
