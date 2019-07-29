#ifndef bool_hpp
#define bool_hpp

// Like POSIX returning 0 for success, failure is 'true'

enum boolean : bool
{
	success = false, failure = true
};

constexpr bool fail(boolean ok)
{
	return success != ok;
}

#endif // file

