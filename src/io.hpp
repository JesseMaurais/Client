#ifndef io_hpp
#define io_hpp

#include <iostream>
#include <vector>

namespace fmt
{
	using in = std::vector<std::istream>::reference;
	using win = std::vector<std::wistream>::reference;

	using out = std::vector<std::ostream>::reference;
	using wout = std::vector<std::wostream>::reference;

	using io = std::vector<std::iostream>::reference;
	using wio = std::vector<std::wiostream>::reference;
}

#endif
