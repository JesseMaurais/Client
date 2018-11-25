#ifndef tool_hpp
#define tool_hpp

namespace stl
{
	struct not_copyable
	{	
		not_copyable() = default;
		not_copyable &operator=(const not_copyable &) = delete;
		not_copyable(const not_copyable &) = delete;
	};

	template <typename T, T Min, T Max>
	struct range
	{
		using value_type = T;

		static constexpr auto min = Min;
		static constexpr auto max = Max;
	};
}

#endif // file