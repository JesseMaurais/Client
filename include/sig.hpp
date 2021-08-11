#ifndef sig_hpp
#define sig_hpp "Signals and Sockets"

#include <map>
#include <string>
#include <csignal>
#include <algorithm>
#include <functional>
#include "fmt.hpp"

namespace fwd::sig
{
	template <class slot, class... args> struct socket : fwd::unique
	{
		using signature = void(args...);
		using function = std::function<signature>;
		using container = std::map<slot, function>;
		using size_type = typename container::size_type;

		size_type const invalid = ~size_type(0);

		auto connect(slot id, function f)
		{
			slots.emplace(id, f);
			return slots.size();
		}

		auto disconnect(slot id)
		{
			return slots.erase(id);
		}

		auto disconnect()
		{
			auto const sz = slots.size();
			slots.clear();
			return sz;
		}

		size_type find(slot id) const
		{
			auto const it = slots.find(id);
			auto const begin = slots.begin();
			auto const end = slots.end();
			return end != it ? distance(begin, it) : invalid;
		}

		void raise(args... a) const
		{
			raise([=](auto const &pair){ pair.second(a...); });
		}

		template <class filter>	void raise(filter &&rule) const
		{
			for_each(begin(slots), end(slots), rule);
		}

		template <class filter>	void raise(slot id, filter &&rule) const
		{
			auto const pair = slots.equal_range(id);
			for_each(pair.first, pair.second, rule);
		}

	protected:

		container slots;
	};

	template <class... args> class slot
	{
	public:

		using socket = socket<slot*, args...>;
		using function = typename socket::function;
		using signature = typename socket::signature;

		slot(socket *s, function f) : that(s)
		{
			that->connect(this, f);
		}

		~slot()
		{
			that->disconnect(this);
		}

	private:

		socket *that;
	};
}

namespace sys::sig
{
	fmt::string::view text(int);

	using slot = fwd::sig::slot<int>;
	using socket = slot::socket;
	using function = slot::function;
	using signature = slot::signature;
	using handler = fwd::as_ptr<signature>;

	inline bool fail(handler h)
	{
		return SIG_ERR == h;
	}

	inline bool fault(handler h)
	{
		return SIG_DFL == h;
	}

	inline bool ignore(handler h)
	{
		return SIG_IGN == h;
	}

	inline bool empty(handler h)
	{
		return fail(h) or fault(h) or ignore(h);
	}

	struct state : fwd::unique, slot
	{
		state(int n, function f) : slot(event(n), f)
		{
			old.n = n;
			old.h = std::signal(n, raise);
			#ifdef alert
			alert(fail(old.h));
			#endif
		}

		~state()
		{
			old.h = std::signal(old.n, old.h);
			#ifdef alert
			alert(fail(old.h));
			#endif
		}

	protected:

		struct {
			handler h;
			int n;
		} old;

	private:

		static socket* event(int);
		static void raise(int);
	};

	struct stack : state
	{
		stack(int no, function f) : state(no, next(f))
		{ }

	protected:

		function next(function f)
		{
			return [=](int n)
			{
				f(n); 
				old.h(n);
			};
		}
	};
}

#endif // file
