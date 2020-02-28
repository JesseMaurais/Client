#ifndef dir_hpp
#define dir_hpp

#include "str.hpp"
#include "env.hpp"
#include "tmp.hpp"
#include "file.hpp"

namespace fmt::path
{
	vector_view split(string_view);
	string join(span_view);
	string join(list_view);
}

namespace fmt::dir
{
	vector_view split(string_view);
	string join(span_view);
	string join(list_view);
}

namespace env::dir
{
	using entry = predicate<fmt::string_view>;

	entry mask(file::mode);
	entry regx(fmt::string_view);
	entry to(fmt::string &);
	entry to(fmt::vector_string &);

	constexpr auto stop = always<fmt::string_view>;
	constexpr auto skip = never<fmt::string_view>;

	static auto // access
		ex = mask(file::ex),
		wr = mask(file::wr),
		rd = mask(file::rd),
		ok = mask(file::ok),
		rw = mask(file::rw),
		rwx = mask(file::rwx);

	inline auto all(fmt::string_view u, file::mode m = file::ok, entry e = skip)
	{
		return mask(m) || regx(u) || e;
	}

	inline auto any(fmt::string_view u, file::mode m = file::ok)
	{
		return all(u, m, stop);
	}

	extern env::pair const& paths;
	extern env::pair const& config;
	extern env::pair const& data;

	bool find(fmt::string_view, entry);
	bool find(fmt::span_view, entry);
	bool find(fmt::pair_view_span, entry);

	bool fail(fmt::string_view path, file::mode = file::ok);
	fmt::string_view make(fmt::string_view path);
	bool remove(fmt::string_view path);

	class tmp
	{
		fmt::string_view stem;

	public:

		tmp(fmt::string_view path)
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
