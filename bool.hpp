#ifndef bool_hpp
#define bool_hpp

namespace etc
{
	enum boolean : bool
	{
		success = false, failure = true
	};

	constexpr bool fail(boolean ok)
	{
		return success != ok;
	}
}

#endif // file

