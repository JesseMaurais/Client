#ifndef ptr_hpp
#define ptr_hpp "Memory & Pointers"

#include "fwd.hpp"
#include <memory>
#include <functional>
#include <type_traits>
#include <span>

namespace fwd
{
	template <class Type> using deleter = std::function<void(as_ptr<Type>)>;
	template <class Type> using unique_ptr = std::unique_ptr<Type, deleter<Type>>;
	template <class Type> using shared_ptr = std::shared_ptr<Type>;
	template <class Type> using weak_ptr = std::weak_ptr<Type>;
	template <class Type> constexpr as_ptr<Type> null = nullptr;

	template <class Type, class Pointer> inline auto cast_as(Pointer ptr)
	{
		#ifdef assert
		assert(nullptr != ptr);
		#endif
		return reinterpret_cast<as_ptr<Type>>(ptr);
	}

	template <class Type> auto non_const(const Type* ptr)
	{
		#ifdef assert
		assert(nullptr != ptr);
		#endif
		return const_cast<as_ptr<Type>>(ptr);
	}

	template <class Type> std::span<Type> to_span(std::initializer_list<Type> in)
	{
		return { non_const(in.begin()), non_const(in.end()) };
	}

	template <class Type, int Byte = 0> struct zero : Type
	{
		static_assert(std::is_trivially_copyable<Type>::value);
		static_assert(std::is_standard_layout<Type>::value);
		zero() { std::memset(this, Byte, sizeof(Type)); }
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

	struct no_copy
	{
		no_copy(const no_copy &) = delete;
		no_copy & operator = (const no_copy &) = delete;
	protected:
		no_copy() = default;
	};

	struct no_move
	{
		no_move(no_move &&) = delete;
	protected:
		no_move() = default;
	};

	struct no_make
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
		class Type
	>
	auto make_shared(as_ptr<Type> ptr, deleter<Type> free = std::default_delete<Type>())
	{
		return shared_ptr<Type>(ptr, free);
	}

	template
	<
		class Type
	>
	auto make_unique(as_ptr<Type> ptr, deleter<Type> free = std::default_delete<Type>())
	{
		return unique_ptr<Type>(ptr, free);
	}

	template
	<
		class Type
	>
	auto null_shared(deleter<Type> free = std::default_delete<Type>())
	{
		return make_shared(null<Type>, free);
	}

	template
	<
		class Type
	>
	auto null_unique(deleter<Type> free = std::default_delete<Type>())
	{
		return make_unique(null<Type>, free);
	}

	template
	<
		class Type, class Free = deleter<Type>
	>
	auto share_ptr(std::enable_shared_from_this<Type>* that, Free free = std::default_delete<Type>())
	{
		auto ptr = that->shared_from_this();
		auto raw = ptr.get();
		ptr.reset();
		return shared_ptr<Type>(raw, free);
	}

	template
	<
		class Type
	>
	struct shared : no_copy, no_make, std::enable_shared_from_this<Type>
	{
		auto share_this(deleter<Type> free = std::default_delete<Type>())
		{
			return share_ptr(this, free);
		}
	};

	template <class Type> struct source : std::pair<Type, shared_ptr<Type>>
	{
		using pair = std::pair<Type, shared_ptr<Type>>;
		using pair::pair;
		operator Type() const
		{
			return pair::first;
		}
	};
}

#endif // file
