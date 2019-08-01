#ifndef ios_hpp
#define ios_hpp

#include <sstream>

namespace io
{
	template <class Char>
	inline bool fail(std::basic_ios<Char> const& ios)
	{
		return not ios;
	}

	struct eat_streambuf : std::stringstream
	{
		eat_streambuf(std::ostream& stream)
		: os(&stream), buf(stream.rdbuf())
		{
			os->rdbuf(rdbuf());
		}

		~eat_streambuf()
		{
			os->rdbuf(buf);
		}

	private:

		std::ostream* const os;
		std::streambuf* const buf;
	};
}

#endif // file
