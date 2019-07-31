#ifndef bool_hpp
#define bool_hpp

enum boolean : bool
{
	success = false, failure = true
};

constexpr bool fail(boolean ok)
{
	return success != ok;
}

#endif // file

