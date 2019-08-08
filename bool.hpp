#ifndef bool_hpp
#define bool_hpp

enum : bool
{
	success = false, failure = true
};

constexpr bool fail(bool ok)
{
	return success != ok;
}

#endif // file

