#ifndef ini_hpp
#define ini_hpp

#include <string_view>
#include <utility>
#include <vector>
#include <map>
#include <ios>

class ini
{
	using ios = std::ios;
	using view = std::string_view;
	using pair = std::pair<view, view>;
	using keys = std::vector<pair>;
	using section = std::pair<view, keys>;
	using content = std::vector<section>;

	view string;
	content data;

public:


};

#endif file
