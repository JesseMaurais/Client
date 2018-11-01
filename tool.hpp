#ifndef tool_hpp
#define tool_hpp

namespace stl
{
	struct not_copyable
	{	
		not_copyable &operator=(const not_copyable &) = delete;
		not_copyable(const not_copyable &) = delete;
	};
}

#endif // file
