#ifndef sig_hpp
#define sig_hpp "Signals and Sockets"

#include <map>
#include <csignal>
#include <functional>
#include <algorithm>
#include "fmt.hpp"

namespace fwd::sig
{
	template <class sink, class... argv> struct socket : fwd::no_copy
	{
		using signature = void(argv...);
		using function = std::function<signature>;
		using container = std::map<sink, function>;

		auto connect(sink id, function f)
		{
			slots.emplace(id, f);
			return slots.size();
		}

		auto disconnect(sink id)
		{
			return slots.erase(id);
		}

		auto disconnect()
		{
			auto const sz = slots.size();
			slots.clear();
			return sz;
		}

		auto find(sink id) const
		{
			auto const it = slots.find(id);
			auto const begin = slots.begin();
			auto const end = slots.end();
			return end != it ? std::distance(begin, it) : fmt::npos;
		}

		void raise(argv... argc) const
		{
			raise([=](auto const &pair){ pair.second(argc...); });
		}

		template <class filter>	void raise(filter rule) const
		{
			std::for_each(begin(slots), end(slots), rule);
		}

		template <class filter>	void raise(sink id, filter rule) const
		{
			auto const pair = slots.equal_range(id);
			std::for_each(pair.first, pair.second, rule);
		}

	protected:

		container slots;
	};

	template <class... args> class slot
	{
	public:

		using sink = as_ptr<slot>;
		using socket = socket<sink, args...>;
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

	struct state : fwd::no_copy, slot
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

	fwd::span<int> native();
	fwd::vector<state> error(fwd::span<int> = native());
	fmt::string::view text(int);
}

#endif // file
