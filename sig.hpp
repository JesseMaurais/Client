#ifndef sig_hpp
#define sig_hpp

#include <algorithm>
#include <functional>
#include <map>

namespace sig
{
	template <typename Slot, typename... Args> class subject
	{
	public:

		using signature = void(Args...);
		using observer = std::function<signature>;
		using container = std::map<Slot, observer>;
		using count = typename container::size_type;

		virtual void connect(Slot const& id, observer fn)
		{
			slots.emplace(id, fn);
		}

		virtual count disconnect(Slot const &id)
		{
			return slots.erase(id);
		}

		void disconnect()
		{
			slots.clear();
		}

		bool find(Slot const &id) const
		{
			return slots.find(id) != slots.end();
		}

		void send(Args... args) const
		{
			send([&](auto const &pair){ pair.second(args...); });
		}

		template <typename Filter>
		void send(Filter &&filter) const
		{
			std::for_each(begin(slots), end(slots), filter);
		}

		template <typename Filter>
		void send(Slot const &id, Filter &&filter) const
		{
			const auto pair = slots.equal_range(id);
			std::for_each(pair.first, pair.second, filter);
		}

	protected:

		container slots;
	};

	template <typename... Args> class slot
	{
	public:

		using subject = ::sig::subject<slot*, Args...>;
		using signature = typename subject::signature;
		using observer = typename subject::observer;

		slot(subject *sub, observer ob) : that(sub)
		{
			that->connect(this, ob);
		}

		~slot()
		{
			that->disconnect(this);
		}

	private:

		subject *that;
	};
}

namespace sys::sig
{
	using slot = ::sig::slot<int>;
	using subject = slot::subject;
	using signature = slot::signature;
	using observer = slot::observer;

	struct scope : slot
	{
		operator bool();
		slot(int no, observer fn);
		~slot();

	protected:

		using base = ::sig::slot<int>;

		struct {
			signature *fn;
			int no;
		} old;
	};

	struct link : scope
	{
		link(int no, observer fn);

	protected

		observer next(observer);
	};

	struct only : link
	{
		only(int no, observer fn) : slot(no, fn)
		{
			assert(nullptr == old.fn);
		}
	};
}

#endif // file
