#ifndef ptr_hpp
#define ptr_hpp "Memory & Pointers"

#include <memory>
#include <functional>
#include <type_traits>

namespace fwd
{
	template <class Type> using as_ptr = typename std::add_pointer<Type>::type;
	template <class Type> using as_ref = typename std::add_lvalue_reference<Type>::type;
	template <class Type> using deleter = std::function<void(as_ptr<Type>)>;
	template <class Type> using unique_ptr = std::unique_ptr<Type, deleter<Type>>;
	template <class Type> using shared_ptr = std::shared_ptr<Type, deleter<Type>>;
	template <class Type> constexpr as_ptr<Type> null = nullptr;
	
	constexpr auto voidptr = null<void>;

	template <class Type, class Pointer> inline auto cast_as(Pointer ptr)
	{
		#ifdef assert
		assert(nullptr != ptr);
		#endif
		return reinterpret_cast<as_ptr<Type>>(ptr);
	}

	template <class Type, int Byte = 0> struct zero : Type // clear trivial class
	{
		static_assert(std::is_trivially_copyable<Type>::value);
		static_assert(std::is_standard_layout<Type>::value);

		zero() { std::memset(this, Byte, sizeof(Type)); }
	};

	template <auto f> class offset_of // pointer-to-member types
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

	struct no_copy // cannot copy base class
	{
		no_copy(const no_copy &) = delete;
		no_copy & operator = (no_copy const &) = delete;
	protected:
		no_copy() = default;
	};

	struct no_move // cannot move base class
	{
		no_move(no_move &&) = delete;
	protected:
		no_move() = default;
	};

	struct no_make // cannot instantiate
	{
		void* operator new (size_t) = delete;
		void operator delete (void*) = delete;
		void* operator new[] (size_t) = delete;
		void operator delete[] (void*) = delete;
	protected:
		no_make() = default;
	};

	template
	<
		class Type, class Free = deleter<Type>
	>
	auto make_shared(as_ptr<Type> ptr, Free del = std::default_delete<Type>())
	{
		return std::shared_ptr<Type, Free>(ptr, del);
	}

	template
	<
		class Type, class Free = deleter<Type>
	>
	auto make_unique(as_ptr<Type> ptr, Free del = std::default_delete<Type>())
	{
		return std::unique_ptr<Type, Free>(ptr, del);
	}

	template
	<
		class Pointer, class Free = deleter<Pointer>
	>
	auto null_shared(Free del = std::default_delete<Pointer>())
	{
		return make_shared((Pointer) nullptr, del);
	}

	template
	<
		class Pointer, class Free = deleter<Pointer>
	>
	auto null_unique(Free del = std::default_delete<Pointer>())
	{
		return make_unique((Pointer) nullptr, del);
	}

	template
	<
		class Type, class Free = deleter<Type>
	>
	auto share_ptr(std::enable_shared_from_this<Type>* that, Free free = std::default_delete<Type>())
	{
		auto ptr = this->shared_from_this();
		auto that = ptr.get();
		ptr.reset();
		return make_ptr(that, free);
	}

	template
	<
		class Type, class Remove = deleter<Type>
	>
	struct shared : no_copy, no_make, std::enable_shared_from_this<Type>
	{
		using Type::Type;

		auto share_this(Remove free = std::default_delete<Type>())
		{
			return share_ptr(this);
		}
	};

	template
	<
		class Type, class Remove = deleter<Type>
	>
	struct closed : no_copy, no_make, std::pair<Type, const Remove>
	{
		using pair::pair;
		
		~closed()
		{
			this->second(this->first);
		}
		
		auto & get() const
		{
			return this->first;
		}
		
		auto & get()
		{
			return this->first;
		}
		
		auto & get_deleter() const
		{
			return this->second;
		}
	};

	struct scoped : std::function<void()>
	{
		using function::function;
	protected:
		~scoped()
		{
			if (*this) operator();
		}
	};
}

#endif // file
