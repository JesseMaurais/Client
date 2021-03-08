#ifndef doc_hpp
#define doc_hpp "Document Structure"

#include <tuple>
#include "fmt.hpp"

namespace doc
{
	template <class... T>
	auto make(std::pair<fmt::string::view, T>... pairs)
	{
		return std::tuple { std::pair { pairs }... };
	}
}

#endif // file