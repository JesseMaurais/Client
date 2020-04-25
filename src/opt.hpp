#ifndef opt_hpp
#define opt_hpp "Program Options"

#include "env.hpp"
#include "fmt.hpp"

namespace env::opt
{
	using fmt::string;
	using string::view;
	using view::span;
	using view::vector;
	using view::pair;
	using string::in;
	using string::out;
	using word = short;
	using quad = long double;
	using vector = fwd::vector<word>;

	in::ref get(in::ref);
	out::ref put(out::ref);

	bool get(view key, bool value);
	bool get(pair key, bool value);
	bool put(view key, bool value);
	bool put(pair key, bool value);
	word get(view key, word value, int base = 10);
	word get(pair key, word value, int base = 10);
	bool put(view key, word value, int base = 10);
	bool put(pair key, word value, int base = 10);
	quad get(view key, quad value);
	quad get(pair key, quad value);
	bool put(view key, quad value);
	bool put(pair key, quad value);
	vector get(view key, span value);
	vector get(pair key, span value);
	bool put(view key, span value);
	bool put(pair key, span value);

	template <typename T> class meme
	{
		pair key;
		T value;

	public:

		meme(pair k, T v) : key(k)
		{
			value = get(k, v);
		}

		~meme()
		{
			put(key, value);
		}

		operator T() const
		{
			return value;
		}

		auto operator=(T v)
		{
			return value = v;
		}
	};
};

#endif // file
