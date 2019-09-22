#ifndef dir_hpp
#define dir_hpp

#include "str.hpp"
#include "fun.hpp"
#include "file.hpp"

namespace fmt::path
{
	views split(view);
	string join(span);
}

namespace fmt::dir
{
	views split(view);
	string join(span);
}

namespace env::dir
{
	using entry = predicate<fmt::view>;
	constexpr auto stop = falsity<fmt::view>;

	entry copy(fmt::string&);
	entry match(fmt::view);
	entry insert(fmt::strings&);
	entry mode(::sys::file::mode);

	extern bool find(fmt::view dir, entry visit);
	inline bool find(fmt::span list, entry visit)
	{
		for (auto const dir : list)
		{
			if (find(dir, visit))
			{
				return true;
			}
		}
		return false;
	}

	fmt::view make(fmt::view path);
	bool remove(fmt::view path);
	bool fail(fmt::view path, ::sys::file::mode = ::sys::file::ok);

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
