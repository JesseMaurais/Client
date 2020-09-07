#ifndef doc_hpp
#define doc_hpp

#include <cstdlib>
#include "fwd.hpp"

namespace doc
{
	template <class Pointer> auto assign(Pointer id) noexcept
	// Inverts the field if the sign bit is disabled
	{
		return std::signbit(id) ? ~id : id;
	}

	template <class Pointer> auto unsign(Pointer id) noexcept
	// Inverts the field if the sign bit is enabled
	{
		return std::signbit(id) ? id : ~id;
	}

	template
	<
		class Record, class Pointer = std::ptrdiff_t
	>
	struct object 
	{
		Pointer const id;

		~object()
		{
			end(id);
		}

		object(Record && that)
		{
			id = begin(that);
		}

		object(object that)
		{
			id = assign(that.id);
		}

		object(object && that)
		{
			id = that.id;
			that.id = assign(id);
		}

		auto const operator->() const
		{
			return &at(unsign(id));
		}

		auto operator->()
		{
			return &at(unsign(id));
		}

		operator auto const &() const
		{
			return at(unsign(id));
		}

		operator auto &()
		{
			return at(unsign(id));
		}

	private:

		extern static Pointer begin(Record && that);
		extern static Record & at(Pointer);
		extern static void end(Pointer);
	};

	using path = object<long, fwd::span<fwd::pair<long>>>;
}

#endif
