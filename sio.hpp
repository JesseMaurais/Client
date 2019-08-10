#ifndef sio_hpp
#define sio_hpp

#include <iostream>
#include <sstream>
#include <fstream>
#include "ptr.hpp"

namespace sio
{
	using base = std::ios_base;
	using in = std::istream;
	using out = std::ostream;
	using io = std::iostream;
	using buf = std::streambuf;
	using str = std::stringstream;

	inline out & err = std::cerr;
	inline out & log = std::clog;

	struct eat : str, unique
	{
		eat(::sio::out & e = err) : wo(e), rd(e.rdbuf())
		{
			wo.rdbuf(rdbuf());
		}

		~eat()
		{
			wo.rdbuf(rd);
		}

	private:

		::sio::out& wo;
		::sio::buf* rd;
	};

	inline bool flush(str & s)
	{
		auto const& ss = s.str();
		bool any = not empty(ss);
		if (any) log << ss;
		return any;
	}
}

#endif // file
