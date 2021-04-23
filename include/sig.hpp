#ifndef sig_hpp
#define sig_hpp "Signals and Sockets"

#include <map>
#include <string>
#include <csignal>
#include <algorithm>
#include <functional>

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

		void raise(args... args) const
		{
			raise([=](auto const &pair){ pair.second(args...); });
		}

		template <class filter>	void raise(filter &&filter) const
		{
			for_each(begin(slots), end(slots), filter);
		}

		template <class filter>	void raise(slot id, filter &&filter) const
		{
			auto const pair = slots.equal_range(id);
			for_each(pair.first, pair.second, filter);
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
	inline bool fail(sighandler_t* f)
	{
		return SIG_ERR == f;
	}

	using slot = fwd::sig::slot<int>;
	using socket = slot::socket;
	using function = slot::function;
	using signature = slot::signature;

	std::string to_string(int);

	struct scope : fwd::unique, slot
	{
		scope(int no, function f) : slot(&event(no), f)
		{
			old.no = no;
			old.f = std::signal(no, raise);
			#ifdef alert
			alert(fail(old.f));
			#endif
		}

		~scope()
		{
			(void) std::signal(old.on, old.f);
		}

	protected:

		struct {
			sighandler_t* f;
			int no;
		} old;

	private:

		static socket& event(int);
		static void raise(int);
	};

	struct stack : scope
	{
		stack(int on, observer ob) : scope(on, next(ob))
		{ }

	protected:

		observer next(observer ob)
		{
			return [=](int on)
			{
				ob(on); old.ob(on);
			};
		}
	};
}

#endif // file
