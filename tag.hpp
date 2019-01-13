#ifndef tag_hpp
#define tag_hpp

#include "fmt.hpp"

namespace tag
{
	using fmt::string;
	using fmt::string_view;

	class format
	{
	public:

		format(string_view s, string_view begin_tag = "{", string_view end_tag = "}")
			: buffer(to_string(s))
			, begin(begin_tag)
			, end(end_tag)
			, index(0)
		{}

		template <typename T> format& operator % (T&& arg)
		{
			buffer = fmt::replace(buffer, next_tag(), to_string(arg));
			return *this;
		}

		operator string_view() const
		{
			return buffer;
		}

		operator string()
		{
			return buffer;
		}

	private:

		string buffer, begin, end;
		string::size_type index;

		string next_tag()
		{
			std::stringstream tag;
			tag << begin << ++index << end;
			return tag.str();
		}
	};
}

#endif // file
