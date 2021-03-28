#ifndef ptr_hpp
#define ptr_hpp "Object Pointers"

#include <memory>
#include <functional>
#include <type_traits>

namespace fwd
{
	template <class Type> using as_ptr = typename std::add_pointer<Type>::type;
	template <class Type> using as_ref = typename std::add_lvalue_reference<Type>::type;
	template <class Type> using deleter = std::function<void(as_ptr<Type>)>;
	template <class Type> using extern_ptr = std::unique_ptr<Type, deleter<Type>>;
	template <class Type> constexpr auto null = static_cast<as_ptr<Type>>(nullptr);

	template <class Type, class Pointer> inline auto cast_as(Pointer ptr)
	{
		#ifdef assert
		assert(nullptr != ptr);
		assert(nullptr != dynamic_cast<as_ptr<Type>>(ptr));
		#endif
		return reinterpret_cast<as_ptr<Type>>(ptr);
	}

	struct unique
	{
		unique(const unique &) = delete;
		unique & operator = (unique const &) = delete;
	protected:
		unique() = default;
	};

	struct local
	{
		void* operator new (size_t) = delete;
		void operator delete (void*) = delete;
		void* operator new[] (size_t) = delete;
		void operator delete[] (void*) = delete;
	};

	template 
	<
		class Type, class Free = deleter<Type>
	>
	auto make_ptr(Type* ptr, Free del = std::default_delete<Type>())
	{
		return std::unique_ptr<Type, Free>(ptr, del);
	}

	template 
	<
		class Pointer, class Free = deleter<Pointer>
	>
	auto null_ptr(Free del = std::default_delete<Pointer>())
	{
		return make_ptr((Pointer) nullptr, del);
	}

	template
	<
		class Type, class Remove = deleter<Type>
	>
	struct shared : unique, local, std::enable_shared_from_this<Type>
	{
		using Type::Type;

		auto extern_from_this(Remove free = std::default_delete<Type>())
		{
			auto ptr = this->shared_from_this();
			auto that = ptr.get();
			ptr.reset();
			return make_ptr(that, free);
		}
	};

	template <auto f> class offset_of
	{
		static_assert(std::is_member_object_pointer<decltype(f)>::value);
		template <class T, class C> static std::pair<T, C> pair(T C::*);

	public:
	
		using pair_type = decltype(pair(f));
		using value_type = typename pair_type::first_type;
		using parent_type = typename pair_type::second_type;
		static constexpr auto value_size = sizeof(value_type);
		static constexpr auto parent_size = sizeof(parent_type);
	};
}

#endif // file
