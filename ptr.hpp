#ifndef ptr_hpp
#define ptr_hpp

#include <memory>

namespace etc
{
	template <typename Type, typename Remove>
	inline auto make_ptr(Type* ptr, Remove rm)
	{
		return std::unique_ptr<Type, Remove>(ptr, rm);
	}

	template <typename Type, typename Remove>
	inline auto null_ptr(Remove rm)
	{
		return make_ptr((Type) nullptr, rm);
	}

	template <typename Type>
	class counter
	{
		Type *ptr;

	public:

		counter(Type& ref) : ptr(&ref)
		{
			++ *ptr;
		}

		~counter()
		{
			-- *ptr;
		}
	};

	template <typename Type>
	class store
	{
		Type *ptr;
		Type value;

	public:

		store(Type& ref) : ptr(&ref)
		{
			value = *ptr;
		}

		~store()
		{
			*ptr = value;
		}
	};

	template <typename Type>
	class stack : store<Type>
	{
		using base = store<Type>;

	public:

		stack(Type& ref, Type val) : base(ref)
		{
			ref = val;
		}
	};
}

#endif // file
