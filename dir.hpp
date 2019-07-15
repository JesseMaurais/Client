#ifndef dir_hpp
#define dir_hpp

#include "env.hpp"
#include <functional>
struct dirent;

namespace sys::file
{
	enum class mode : int
	{
		ok = 1, read = 2, write = 4, run = 8
	};

	int convert(mode);

	inline mode operator | (mode L, mode R)
	{
		return static_cast<mode>(static_cast<int>(L)|static_cast<int>(R));
	}
}

namespace sys::dir
{
	fmt::string join(fmt::string_view_span);
	fmt::string_view_vector split(fmt::string_view);
}

namespace sys::path
{
	fmt::string join(fmt::string_view_span);
	fmt::string_view_vector split(fmt::string_view);
}

namespace env::dir
{
	extern env::list const& bin;
	extern env::list const& share;
	extern env::list const& include;

	struct view : std::function<bool(dirent const*)>
	{
		using base = std::function<bool(dirent const*)>;

		view operator | (base const& that) const
		{
			return [&](dirent const* ent)
			{
				return this->operator()(ent) and that(ent);
			};
		}
	};

	view mode(sys::file::mode);
	view regex(fmt::string_view);
	view name(fmt::string&);

	bool find(fmt::string path, view);

	inline bool find(fmt::string_view_span list, view)
	{
		for (auto const path : list)
		{
			if (find(path, view))
			{
				return true;
			}
		}
		return false;
	}

	inline bool find_bin(fmt::string_view binary)
	{
		return find(bin, regex(binary) | mode(sys::file::run));
	}

	inline bool find_obj(fmt::string_view object)
	{
		return find(share, regex(object) | mode(sys::file::read));
	}

	inline bool find_src(fmt::string_view header)
	{
		return find(include, regex(header) | mode(sys::file::read));
	}
}

#endif // file
